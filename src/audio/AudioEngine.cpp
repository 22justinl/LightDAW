#include "audio/AudioEngine.h"

#include <juce_audio_formats/juce_audio_formats.h>

#include <string>

constexpr char default_device[] = "MacBook Pro Microphone";
constexpr ChannelId MAX_INPUT_CHANNELS = 8;
constexpr ChannelId MAX_OUTPUT_CHANNELS = 8;

// PUBLIC FUNCTIONS

AudioEngine::AudioEngine(): io_callback(*this), transport(), stream_manager(transport, format_manager, tracks), track_mixer_source(transport, channel_manager, tracks) {
    device_manager.initialise(MAX_INPUT_CHANNELS, MAX_OUTPUT_CHANNELS, nullptr, true, default_device);
    device_manager.addAudioCallback(&io_callback);
    device_manager.addChangeListener(this);

    format_manager.registerBasicFormats();
}

AudioEngine::~AudioEngine() {
    if (is_playing()) {
        stop();
    }
    device_manager.removeAudioCallback(&io_callback);
    device_manager.removeChangeListener(this);
    track_mixer_source.releaseResources();
}

void AudioEngine::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == &device_manager) {
        handle_device_change();
    }
}

TrackId AudioEngine::add_track() {
    tracks.emplace(next_track_id, std::make_unique<Track>(transport, next_track_id, input_view));
    track_ids.push_back(next_track_id);
    return next_track_id++;
}

void AudioEngine::erase_track(TrackId track_id) {
    if (tracks.find(track_id) == tracks.end()) {
        throw std::runtime_error("Track " + std::to_string(track_id) + " does not exist");
    }
    for (size_t i = 0; i < track_ids.size(); ++i) {
        if (track_ids[i] == track_id) {
            track_ids.erase(track_ids.begin() + static_cast<int>(i));
            break;
        }
    }
    tracks.erase(track_id);
}

std::vector<TrackId>& AudioEngine::get_track_ids() {
    return track_ids;
}

std::unordered_map<TrackId, std::unique_ptr<Track>>& AudioEngine::get_tracks() {
    return tracks;
}

void AudioEngine::set_input_view_ptr(const float* const* input_channel_data) {
    input_view.input_channel_data = input_channel_data;
}

// Track interface

float AudioEngine::track_get_volume(TrackId track_id) { return tracks[track_id]->get_volume(); }
void AudioEngine::track_set_volume(TrackId track_id, float new_gain_db) { tracks[track_id]->set_volume(new_gain_db); }

float AudioEngine::track_get_pan(TrackId track_id) { return tracks[track_id]->get_pan(); }
void AudioEngine::track_set_pan(TrackId track_id, float new_pan) { tracks[track_id]->set_pan(new_pan); }

bool AudioEngine::track_is_muted(TrackId track_id) { return tracks[track_id]->is_muted(); }
void AudioEngine::track_set_muted(TrackId track_id, bool new_muted) { tracks[track_id]->set_muted(new_muted); }

bool AudioEngine::track_is_soloed(TrackId track_id) { return tracks[track_id]->is_soloed(); }
void AudioEngine::track_set_soloed(TrackId track_id, bool new_soloed) {
    if (tracks[track_id]->is_soloed() == new_soloed) { return; }
    tracks[track_id]->set_soloed(new_soloed);
    if (new_soloed) {
        track_mixer_source.increment_solo_count();
    } else {
        track_mixer_source.decrement_solo_count();
    }
}

bool AudioEngine::track_is_recording(TrackId track_id) { return tracks[track_id]->is_recording(); }
void AudioEngine::track_set_recording(TrackId track_id, bool new_recording) { tracks[track_id]->set_recording(new_recording); }

bool AudioEngine::track_is_monitoring(TrackId track_id) { return tracks[track_id]->is_monitoring(); }
void AudioEngine::track_set_monitoring(TrackId track_id, bool new_monitoring) { tracks[track_id]->set_monitoring(new_monitoring); }

void AudioEngine::track_import_file(TrackId track_id, const juce::File& file, juce::int64 pos) {
    if (!file.existsAsFile()) { DBG("File doesn't exist"); return; }
    juce::AudioFormatReader* reader = format_manager.createReaderFor(file);
    if (!reader) { DBG("Couldn't open file"); return; }

    tracks[track_id]->import_file(file, pos, 0, reader->lengthInSamples);
    delete reader;
}

void AudioEngine::play() {
    track_mixer_source.prepareToPlay(device_setup.bufferSize, device_setup.sampleRate);
    transport.set_position(0);
    transport.play();
    stream_manager.startThread();
}

void AudioEngine::stop() {
    stream_manager.reset_play_ready();
    transport.stop();
    stream_manager.notify_cv();
    stream_manager.stopThread(2000);
}

void AudioEngine::notify_cv() {
    stream_manager.notify_cv();
}

bool AudioEngine::is_playing() const {
    return transport.is_playing();
}

bool AudioEngine::is_playback_ready() const {
    return stream_manager.is_play_ready();
}

SamplePosition AudioEngine::get_position() const {
    return transport.get_position();
}

void AudioEngine::advance_playhead(SamplePosition change) {
    transport.set_position(transport.get_position() + change);
}

void AudioEngine::check_threshold_and_notify() {
    stream_manager.check_threshold_and_notify();
}

TrackMixerSource& AudioEngine::get_track_mixer_source() {
    return track_mixer_source;
}

ChannelManager& AudioEngine::get_channel_manager() {
    return channel_manager;
}

// PRIVATE FUNCTIONS

void AudioEngine::handle_device_change() {
    device_manager.getAudioDeviceSetup(device_setup);
    channel_manager.update_channels(device_setup);

    for (TrackId track_id : track_ids) {
        TrackMixerControls& mixer_controls = tracks[track_id]->get_mixer_controls();
        mixer_controls.input_buffer_idx = channel_manager.input_channel_to_buffer_idx(mixer_controls.input_src_channel);
        mixer_controls.output_buffer_idx = channel_manager.output_channel_to_buffer_idx(mixer_controls.output_dest_channel);
    }
}
