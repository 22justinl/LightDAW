#pragma once

#include "audio/DAWIOCallback.h"
#include "audio/PlaybackSource.h"
#include "audio/StreamManager.h"
#include "audio/Track.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_events/juce_events.h>
#include <juce_audio_devices/juce_audio_devices.h>

#include <unordered_map>

class AudioEngine: public juce::ChangeListener {
public:
    AudioEngine();
    ~AudioEngine() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    TrackId add_track();
    void erase_track(TrackId track_id);

    std::vector<TrackId>& get_track_ids();
    std::unordered_map<TrackId, std::unique_ptr<Track>>& get_tracks();

    std::pair<ChannelId, ChannelId> output_channels() const;

    // Track interface
    float track_get_gain(TrackId track_id);
    void track_set_gain(TrackId track_id, float new_gain_db);

    bool track_is_muted(TrackId track_id);
    void track_set_mute(TrackId track_id, bool new_muted);

    bool track_is_monitoring(TrackId track_id);
    void track_set_monitoring(TrackId track_id, bool new_monitoring);

    void track_import_file(TrackId track_id, const juce::File& file, SamplePosition position);

    void play();
    void stop();
    void notify_cv();
    bool is_playing() const;
    bool is_playback_ready() const;
    SamplePosition get_position() const;
    void advance_playhead(SamplePosition change);
    PlaybackSource& get_playback_source();

    juce::AudioDeviceManager device_manager;
private:
    void handleDeviceChange();

    void calculate_active_channel_mapping(std::unordered_map<ChannelId, ChannelId>& m, juce::BigInteger active_channel_bits);
    void track_update_channel(TrackId track_id);

    TrackId next_track_id = 0;
    std::unordered_map<TrackId, std::unique_ptr<Track>> tracks;
    std::vector<TrackId> track_ids;

    DAWIOCallback io_callback;
    juce::AudioDeviceManager::AudioDeviceSetup device_setup;
    juce::AudioFormatManager format_manager;
    Transport transport;
    StreamManager stream_manager;
    PlaybackSource playback_source;

    std::unordered_map<ChannelId, ChannelId> input_phys_to_buf_id;
    std::unordered_map<ChannelId, ChannelId> output_phys_to_buf_id;
    ChannelId default_input_channel = -1;

    ChannelId left_output_channel = -1;
    ChannelId right_output_channel = -1;
    ChannelId left_output_buffer_channel = -1;
    ChannelId right_output_buffer_channel = -1;

};
