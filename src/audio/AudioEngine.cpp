#include "audio/AudioEngine.h"

#include <juce_audio_formats/juce_audio_formats.h>

constexpr char default_device[] = "MacBook Pro Microphone";
constexpr ChannelId MAX_INPUT_CHANNELS = 8;
constexpr ChannelId MAX_OUTPUT_CHANNELS = 8;

// PUBLIC FUNCTIONS

AudioEngine::AudioEngine(): io_callback(*this), transport(), stream_manager(transport, format_manager, tracks), playback_source(transport) {
    device_manager.initialise(MAX_INPUT_CHANNELS, MAX_OUTPUT_CHANNELS, nullptr, true, default_device);
    device_manager.addAudioCallback(&io_callback);
    device_manager.addChangeListener(this);

    format_manager.registerBasicFormats();
    // format_manager.registerFormat(new juce::MP3AudioFormat(), false);
}

AudioEngine::~AudioEngine() {
    if (is_playing()) {
        stop();
    }
    device_manager.removeChangeListener(this);
}

void AudioEngine::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == &device_manager) {
        handleDeviceChange();
    }
}

TrackId AudioEngine::add_track() {
    tracks.emplace(next_track_id, std::make_unique<Track>(transport, next_track_id));
    track_ids.push_back(next_track_id);
    track_update_channel(next_track_id);
    playback_source.addInputSource(&tracks[next_track_id]->get_audio_source());
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
    playback_source.removeInputSource(&tracks[track_id]->get_audio_source());
    tracks.erase(track_id);
}

std::vector<TrackId>& AudioEngine::get_track_ids() {
    return track_ids;
}

std::unordered_map<TrackId, std::unique_ptr<Track>>& AudioEngine::get_tracks() {
    return tracks;
}

std::pair<ChannelId, ChannelId> AudioEngine::output_channels() const {
    return {left_output_buffer_channel, right_output_buffer_channel};
}

// Track interface

float AudioEngine::track_get_gain(TrackId track_id) { return tracks[track_id]->get_gain(); }
void AudioEngine::track_set_gain(TrackId track_id, float new_gain_db) { tracks[track_id]->set_gain(new_gain_db); }

bool AudioEngine::track_is_muted(TrackId track_id) { return tracks[track_id]->is_muted(); }
void AudioEngine::track_set_mute(TrackId track_id, bool new_muted) { tracks[track_id]->set_muted(new_muted); }

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
    playback_source.prepareToPlay(device_setup.bufferSize, device_setup.sampleRate);
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

PlaybackSource& AudioEngine::get_playback_source() {
    return playback_source;
}

// PRIVATE FUNCTIONS

void AudioEngine::handleDeviceChange() {
    device_manager.getAudioDeviceSetup(device_setup);
    input_phys_to_buf_id.clear();
    output_phys_to_buf_id.clear();

    // DBG("Device Changed:");
    if (!device_setup.inputDeviceName.isEmpty()) {
        // DBG("Input channel mapping");
        calculate_active_channel_mapping(input_phys_to_buf_id, device_manager.getCurrentAudioDevice()->getActiveInputChannels());
    }
    if (!device_setup.outputDeviceName.isEmpty()) {
        // DBG("Output channel mapping");
        calculate_active_channel_mapping(output_phys_to_buf_id, device_manager.getCurrentAudioDevice()->getActiveOutputChannels());
    }
    ChannelId active_output_channels = static_cast<ChannelId>(output_phys_to_buf_id.size());

    input_phys_to_buf_id[-1] = -1;
    output_phys_to_buf_id[-1] = -1;

    // DBG("\tInput:\t" + device_setup.inputDeviceName + "\t(" + std::to_string(active_input_channels) + " channels)");
    // DBG("\tOutput:\t" + device_setup.outputDeviceName + "\t(" + std::to_string(active_output_channels) + " channels)");
    // DBG("\tSample rate:\t" + std::to_string(device_setup.sampleRate) + "\tBuffer size:\t" + std::to_string(device_setup.bufferSize));

    default_input_channel = device_setup.inputChannels.findNextSetBit(0);

    for (const auto& p : tracks) {
        track_update_channel(p.first);
    }
    if (active_output_channels == 0) {
        left_output_channel = -1;
        right_output_channel = -1;
    } else {
        left_output_channel = device_setup.outputChannels.findNextSetBit(0);
        right_output_channel = device_setup.outputChannels.findNextSetBit(static_cast<int>(left_output_channel+1));
    }
    left_output_buffer_channel = output_phys_to_buf_id[left_output_channel];
    right_output_buffer_channel = output_phys_to_buf_id[right_output_channel];
    // DBG("Left out: " + std::to_string(left_output_channel) + "->" + std::to_string(left_output_buffer_channel));
    // DBG("Right out: " + std::to_string(right_output_channel) + "->" + std::to_string(right_output_buffer_channel));
}

void AudioEngine::calculate_active_channel_mapping(std::unordered_map<ChannelId, ChannelId>& m, juce::BigInteger active_channel_bits) {
    int active_channels = 0;
    int bit = active_channel_bits.findNextSetBit(0);
    // DBG("\t\tPhysical\tBuffer");
    while (bit != -1) {
        m[bit] = active_channels;
        // DBG("\t\t" + std::to_string(bit) + "\t->\t" + std::to_string(active_channels));
        ++active_channels;
        bit = active_channel_bits.findNextSetBit(bit+1);
    }
}

void AudioEngine::track_update_channel(TrackId track_id) {
    Track& track = *tracks[track_id];
    if (    track.get_selected_device_name() == device_setup.inputDeviceName &&
            track.get_selected_channel() != -1 &&
            input_phys_to_buf_id.find(track.get_selected_channel()) != input_phys_to_buf_id.end()) {
        track.set_channel(track.get_selected_channel());
        track.set_buffer_channel(input_phys_to_buf_id[track.get_selected_channel()]);
    } else {
        track.set_channel(default_input_channel);
        track.set_buffer_channel(input_phys_to_buf_id[default_input_channel]);
    }
}

