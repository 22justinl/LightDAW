#pragma once

#include "Types.h"
#include "audio/TrackMixerControls.h"

#include <juce_audio_devices/juce_audio_devices.h>

class ChannelManager: public juce::ChangeBroadcaster {
public:
    ChannelManager();
    ~ChannelManager();

    void update_channels(const juce::AudioDeviceManager::AudioDeviceSetup& device_setup);

    bool is_input_channel_active(ChannelId channel, ChannelLayout layout) const;
    bool is_output_channel_active(ChannelId channel, ChannelLayout layout) const;

    ChannelId input_channel_to_buffer_idx(ChannelId channel);
    ChannelId output_channel_to_buffer_idx(ChannelId channel);

    ChannelId get_default_input_channel(ChannelLayout layout);
    ChannelId get_default_output_channel(ChannelLayout layout);

    const std::unordered_map<ChannelLayout, juce::StringArray>& get_input_channel_strings() const;
    const std::unordered_map<ChannelLayout, juce::StringArray>& get_output_channel_strings() const;

    const std::pair<ChannelId, ChannelLayout> get_input_channel_from_string(const juce::String& channel_string) const;
    const std::pair<ChannelId, ChannelLayout> get_output_channel_from_string(const juce::String& channel_string) const;
private:
    void clear_channel_info();
    void calculate_active_channel_mapping(
            juce::BigInteger active_channel_bits,
            std::unordered_map<ChannelId, ChannelId>& channel_mapping,
            std::unordered_map<ChannelLayout, juce::StringArray>& layout_channels,
            std::unordered_map<juce::String, std::pair<ChannelId, ChannelLayout>>& string_to_layout_channel,
            std::unordered_map<ChannelLayout, ChannelId>& default_channel);
    juce::String construct_channel_string(ChannelId channel, ChannelLayout layout);

    ChannelLayout layouts[2] = {ChannelLayout::Mono, ChannelLayout::Stereo};

    juce::BigInteger active_inputs;
    juce::BigInteger active_outputs;

    std::unordered_map<ChannelId, ChannelId> input_channel_mapping;
    std::unordered_map<ChannelId, ChannelId> output_channel_mapping;

    std::unordered_map<ChannelLayout, ChannelId> default_input_channel;
    std::unordered_map<ChannelLayout, ChannelId> default_output_channel;

    // options for selection
    std::unordered_map<ChannelLayout, juce::StringArray> input_channel_strings;
    std::unordered_map<ChannelLayout, juce::StringArray> output_channel_strings;

    std::unordered_map<juce::String, std::pair<ChannelId, ChannelLayout>> input_channel_string_to_channel;
    std::unordered_map<juce::String, std::pair<ChannelId, ChannelLayout>> output_channel_string_to_channel;
};
