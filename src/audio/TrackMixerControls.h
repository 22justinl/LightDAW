#pragma once

#include "Types.h"

#include <juce_core/juce_core.h>

#include <atomic>

struct TrackMixerControls {
    std::atomic<float> gain_db = 0;
    std::atomic_bool muted = false;
    std::atomic_bool monitoring = false;

    // TODO: stereo/mono and panning (juce::dsp::Panner)
    // mono also needs panning (to center) not just copies on left and right

    // TODO: do we need atomics here?
    juce::String selected_device_name = "";
    ChannelId selected_channel = -1;
    ChannelId channel = -1;         // use this if current device is not selected device
    ChannelId buffer_channel = -1;  // channel index in callback buffer
};
