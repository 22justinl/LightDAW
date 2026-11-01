#pragma once

#include "audio/DAWIOCallback.h"
#include "audio/TrackMixerSource.h"
#include "audio/StreamManager.h"
#include "audio/Track.h"
#include "audio/AudioInputView.h"

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

    void set_input_view_ptr(const float* const* input_channel_data);

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
    void check_threshold_and_notify();
    TrackMixerSource& get_track_mixer_source();

    juce::AudioDeviceManager device_manager;
private:
    void handle_device_change();

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
    TrackMixerSource track_mixer_source;
    AudioInputView input_view;

    std::unordered_map<ChannelId, ChannelId> input_channel_mapping;
    std::unordered_map<ChannelId, ChannelId> output_channel_mapping;
    ChannelId default_left_input_channel = -1;
    ChannelId default_right_input_channel = -1;
    ChannelId default_left_output_channel = -1;
    ChannelId default_right_output_channel = -1;

    ChannelId left_output_channel = -1;
    ChannelId right_output_channel = -1;
    ChannelId left_output_buffer_channel = -1;
    ChannelId right_output_buffer_channel = -1;

};
