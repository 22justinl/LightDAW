#include "audio/ChannelManager.h"
#include "audio/TrackMixerControls.h"

ChannelManager::ChannelManager() {
    for (ChannelLayout layout : layouts) {
        default_input_channel.emplace(layout, -1);
        default_output_channel.emplace(layout, -1);
        // input_channels[layout] = {};
        // output_channels[layout] = {};
        input_channel_strings[layout] = {};
        output_channel_strings[layout] = {};
    }
}

ChannelManager::~ChannelManager() {
    removeAllChangeListeners();
}

void ChannelManager::update_channels(const juce::AudioDeviceManager::AudioDeviceSetup& device_setup) {
    clear_channel_info();

    active_inputs = device_setup.inputChannels;
    active_outputs = device_setup.outputChannels;

    calculate_active_channel_mapping(
        active_inputs,
        input_channel_mapping,
        input_channel_strings,
        input_channel_string_to_channel,
        default_input_channel);
    calculate_active_channel_mapping(
        active_outputs,
        output_channel_mapping,
        output_channel_strings,
        output_channel_string_to_channel,
        default_output_channel);

    sendChangeMessage();
}

bool ChannelManager::is_input_channel_active(ChannelId channel, ChannelLayout layout) const {
    if (channel < 0) {
        return false;
    }
    switch (layout) {
        case ChannelLayout::Mono:
            return active_inputs[static_cast<int>(channel)];
        case ChannelLayout::Stereo:
            if (channel % 2 == 0) {
                return active_inputs[static_cast<int>(channel)] && active_inputs[static_cast<int>(channel)+1];
            }
            return active_inputs[static_cast<int>(channel)-1] && active_inputs[static_cast<int>(channel)];
    }
}

bool ChannelManager::is_output_channel_active(ChannelId channel, ChannelLayout layout) const {
    if (channel < 0) {
        return false;
    }
    switch (layout) {
        case ChannelLayout::Mono:
            return active_outputs[static_cast<int>(channel)];
        case ChannelLayout::Stereo:
            if (channel % 2 == 0) {
                return active_outputs[static_cast<int>(channel)] && active_outputs[static_cast<int>(channel)+1];
            }
            return active_outputs[static_cast<int>(channel)-1] && active_outputs[static_cast<int>(channel)];
    }
}

ChannelId ChannelManager::input_channel_to_buffer_idx(ChannelId channel) {
    return input_channel_mapping.find(channel) == input_channel_mapping.end() ? -1 : input_channel_mapping[channel];
}

ChannelId ChannelManager::output_channel_to_buffer_idx(ChannelId channel) {
    return output_channel_mapping.find(channel) == output_channel_mapping.end() ? -1 : output_channel_mapping[channel];
}

ChannelId ChannelManager::get_default_input_channel(ChannelLayout layout) {
    return default_input_channel[layout];
}

ChannelId ChannelManager::get_default_output_channel(ChannelLayout layout) {
    return default_output_channel[layout];
}

const std::unordered_map<ChannelLayout, juce::StringArray>& ChannelManager::get_input_channel_strings() const {
    return input_channel_strings;
}

const std::unordered_map<ChannelLayout, juce::StringArray>& ChannelManager::get_output_channel_strings() const {
    return output_channel_strings;
}

const std::pair<ChannelId, ChannelLayout> ChannelManager::get_input_channel_from_string(const juce::String& channel_string) const {
    return input_channel_string_to_channel.at(channel_string);
}

const std::pair<ChannelId, ChannelLayout> ChannelManager::get_output_channel_from_string(const juce::String& channel_string) const {
    return output_channel_string_to_channel.at(channel_string);
}

void ChannelManager::clear_channel_info() {
    input_channel_mapping.clear();
    output_channel_mapping.clear();

    for (ChannelLayout layout : layouts) {
        default_input_channel[layout] = -1;
        default_output_channel[layout] = -1;
        input_channel_strings[layout].clear();
        output_channel_strings[layout].clear();
        input_channel_string_to_channel.clear();
        output_channel_string_to_channel.clear();
    }
}

void ChannelManager::calculate_active_channel_mapping(
        juce::BigInteger active_channel_bits,
        std::unordered_map<ChannelId, ChannelId>& channel_mapping,
        std::unordered_map<ChannelLayout, juce::StringArray>& layout_channels,
        std::unordered_map<juce::String, std::pair<ChannelId, ChannelLayout>>& string_to_layout_channel,
        std::unordered_map<ChannelLayout, ChannelId>& default_channel) {
    int active_channels = 0;
    int bit = active_channel_bits.findNextSetBit(0);

    if (bit != -1) {
        default_channel[ChannelLayout::Mono] = bit;
        if (bit % 2 == 0 && active_channel_bits[bit+1]) {
            default_channel[ChannelLayout::Stereo] = bit;
        }
    }

    while (bit != -1) {
        juce::String str = construct_channel_string(bit, ChannelLayout::Mono);
        layout_channels[ChannelLayout::Mono].add(str);
        string_to_layout_channel[str] = {bit, ChannelLayout::Mono};

        if (bit % 2 == 0 && active_channel_bits[bit+1]) {
            str = construct_channel_string(bit, ChannelLayout::Stereo);
            layout_channels[ChannelLayout::Stereo].add(str);
            string_to_layout_channel[str] = {bit, ChannelLayout::Stereo};
        }

        channel_mapping[bit] = active_channels++;
        bit = active_channel_bits.findNextSetBit(bit+1);
    }
}

juce::String ChannelManager::construct_channel_string(ChannelId channel, ChannelLayout layout) {
    switch (layout) {
        case ChannelLayout::Mono:
            return juce::String(channel+1);
        case ChannelLayout::Stereo:
            return juce::String(channel+1) + "/" + juce::String(channel+2);
    }
}
