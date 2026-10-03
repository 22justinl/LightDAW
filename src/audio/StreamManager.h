#pragma once

#include "audio/Track.h"
#include "audio/Transport.h"
#include "utils/SPSCQueue.h"

#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>

#include <atomic>
#include <condition_variable>
#include <queue>

class StreamManager: public juce::Thread {
public:
    StreamManager(Transport& transport_, juce::AudioFormatManager& format_manager_, std::unordered_map<TrackId, std::unique_ptr<Track>>& tracks);

    void run() override;

    void play();
    bool try_queue_refill(Clip* clip);
    bool is_play_ready() const;
    void reset_play_ready();

    void notify_cv();
private:
    void prepare_playback();
    void initial_fill();
    void refill();
    void seek(SamplePosition new_position);

    void prepare_track_playback_states();
    void create_prefetch_streams();
    void remove_finished_streams();
    void remove_deferred_streams();

    int prefetch_blocks = 10;
    int prefetch_window_size = 10 * 4096;
    int stream_blocks = 5;
    int stream_buffer_size = 5 * 4096;
    SamplePosition playback_end = INT_MAX;

    Transport& transport;
    juce::AudioFormatManager& format_manager;
    std::unordered_map<TrackId, std::unique_ptr<Track>>& tracks;

    juce::ThreadPool thread_pool;
    SPSCQueue<Clip*> refill_queue;
    size_t refill_overflow_count = 0;
    std::queue<Clip*> deferred_destruction_queue;

    std::atomic_bool refill_requested{false};
    std::atomic_bool play_ready{false};
    std::atomic<SamplePosition> r_position{0}; // readahead position
    std::atomic<SamplePosition> seek_position{-1};

    std::condition_variable cv;
    std::mutex m;
};
