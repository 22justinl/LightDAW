#pragma once

#include <juce_core/juce_core.h>

using TrackId = std::uint64_t;
using ChannelId = int;

class Track {
public:
    Track(TrackId id_);
    Track(TrackId id_, const juce::String& name);

    juce::String get_name() const;
    void set_name(const juce::String& new_name);

    float get_gain() const;
    void set_gain(float new_gain_db);

    bool is_muted() const;
    void set_muted(bool new_muted);

    bool is_monitoring() const;
    void set_monitoring(bool new_monitoring);

    const juce::String& get_selected_device_name() const;
    void set_selected_device_name(const juce::String& new_device_name);

    ChannelId get_selected_channel() const;
    void set_selected_channel(ChannelId new_channel);

    ChannelId get_channel() const;
    void set_channel(ChannelId new_channel);

    ChannelId get_buffer_channel() const;
    void set_buffer_channel(ChannelId new_channel);


    void import_file(const juce::File& file);
private:
    const TrackId id;
    juce::String name = "New Track";

    float gain_db = 0;
    bool muted = false;
    bool monitoring = false;

    // TODO: stereo/mono and panning (juce::dsp::Panner)
    // mono also needs panning (to center) not just copies on left and right

    juce::String selected_device_name = "";
    ChannelId selected_channel = -1;
    ChannelId channel = -1;         // use this if current device is not selected device
    ChannelId buffer_channel = -1;  // channel index in callback buffer
};
