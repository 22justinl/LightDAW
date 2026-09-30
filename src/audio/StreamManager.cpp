#include "audio/StreamManager.h"

#include "audio/Track.h"

#include <algorithm>
#include <memory>

StreamManager::StreamManager(Transport& transport_, juce::AudioFormatManager& format_manager_, std::unordered_map<TrackId, std::unique_ptr<Track>>& tracks_)
    :   transport(transport_),
        format_manager(format_manager_),
        tracks(tracks_),
        refill_queue(64) {}
void StreamManager::play() {
    prepare_playback();
    create_prefetch_streams();
    initial_fill();
    play_ready.store(true);
    while (transport.is_playing()) {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&](){ return !transport.is_playing() || !refill_queue.empty() || r_position < transport.get_position() + prefetch_window_size || !deferred_destruction_queue.empty(); });
        if (!transport.is_playing()) { break; }
        refill_requested.store(false);
        create_prefetch_streams();
        remove_finished_streams();
        remove_deferred_streams();
        refill();
        initial_fill();
    }
}
bool StreamManager::try_queue_refill(Clip* clip) {
    if (refill_queue.full() || !refill_queue.push(clip)) {
        ++refill_overflow_count;
        return false;
    }
    return true;
}

void StreamManager::prepare_playback() {
    prefetch_window_size = prefetch_blocks * 4096; // buffer size
    stream_buffer_size = stream_blocks * 4096; // buffer size
    r_position.store(transport.get_position());
    // TODO: calculate playback end

    while (!refill_queue.empty()) {
        refill_queue.clear();
    }

    // TODO: support different initial position
    for (auto& p : tracks) {
        Track& track = *p.second;
        track.playback_state.curr_clip = 0;
        track.playback_state.next_clip = 0;
        track.playback_state.prefetch_clip = 0;
        for (auto& c : track.clips) {
            c.stream = nullptr;
        }
    }
}

void StreamManager::initial_fill() {
    int curr_position = transport.get_position();

    while (r_position < std::min(curr_position + prefetch_window_size, playback_end)) {
        // interupt fill if seeking
        if (seek_position.load() != -1) {
            return;
        }
        for (auto& p : tracks) {
            Track& track = *p.second;
            std::vector<Clip>& clips = track.clips;
            size_t& next_clip = track.playback_state.next_clip;
            size_t& prefetch_clip = track.playback_state.prefetch_clip;
            while (prefetch_clip < next_clip && clips[prefetch_clip].pos < r_position) {
                if (clips[prefetch_clip].stream) { DBG("prefetch_clip doesn't have stream"); return; }
                if (!clips[prefetch_clip].stream->is_initial_fill_queued()) {
                    // queue fill
                    clips[prefetch_clip].stream->fill_in_progress.store(true);
                    thread_pool.addJob(new AudioFileStreamFill(clips[prefetch_clip]), true); 
                }
                ++prefetch_clip;
            }
        }
        r_position += 4096; // buffer size
    }
}

void StreamManager::refill() {
    Clip* clip_ptr = nullptr;
    while (!refill_queue.empty()) {
        refill_queue.pop(clip_ptr);
        if (clip_ptr->end_pos < transport.get_position()) {
            continue;
        }
        // queue refills
        clip_ptr->stream->fill_in_progress.store(true);
        thread_pool.addJob(new AudioFileStreamFill(*clip_ptr), true); 
    }
}

void StreamManager::seek(int new_position) {
    // TODO: seeking
    DBG("seek to " + std::to_string(new_position));
}

void StreamManager::create_prefetch_streams() {
    int prefetch_right = std::min(transport.get_position() + prefetch_window_size, playback_end);

    for (auto& p : tracks) {
        Track& track = *p.second;
        std::vector<Clip>& clips = track.clips;
        size_t& next_clip = track.playback_state.next_clip;

        // create streams for clips that just entered window
        while (next_clip < clips.size() && clips[next_clip].pos < prefetch_right) {
            if (!clips[next_clip].stream) {
                clips[next_clip].stream = std::make_unique<AudioFileStream>(2, stream_buffer_size, format_manager, clips[next_clip], *this);
            }
            ++next_clip;
        }
    }
}

void StreamManager::remove_finished_streams() {
    int curr_pos = transport.get_position();

    for (auto& p : tracks) {
        Track& track = *p.second;
        std::vector<Clip>& clips = track.clips;
        const size_t last_clip = track.playback_state.next_clip;
        size_t& curr_clip = track.playback_state.curr_clip;

        while (curr_clip < last_clip && clips[curr_clip].end_pos < curr_pos) {
            if (clips[curr_clip].stream) {
                if (clips[curr_clip].stream->fill_in_progress.load()) {
                    deferred_destruction_queue.push(&clips[curr_clip]);
                } else {
                    clips[curr_clip].stream = nullptr;
                }
            }
            ++curr_clip;
        }
    }
}

void StreamManager::remove_deferred_streams() {
    size_t n = deferred_destruction_queue.size();
    for (size_t i = 0; i < n; ++i) {
        Clip* clip = deferred_destruction_queue.front();
        deferred_destruction_queue.pop();
        if (clip->stream->fill_in_progress.load()) {
            clip->stream = nullptr;
        } else {
            deferred_destruction_queue.push(clip);
        }
    }
}
