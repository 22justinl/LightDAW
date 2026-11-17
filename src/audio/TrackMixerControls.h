#pragma once

#include "Types.h"

#include <juce_core/juce_core.h>

#include <atomic>

enum class InputSource {
    External = 1
};

enum class OutputDestination {
    Master = 1,
    External = 2
};

enum class ChannelLayout {
    Mono,
    Stereo
};

struct TrackMixerControls {
    std::atomic<float> gain_db = 0;
    std::atomic_bool muted = false;
    std::atomic_bool monitoring = false;

    // TODO: stereo/mono and panning (juce::dsp::Panner)
    // mono also needs panning (to center) not just copies on left and right

    // Input Info
    std::atomic<InputSource> input_src = InputSource::External;
    std::atomic<ChannelId> input_src_channel = 0;
    std::atomic<ChannelLayout> input_layout = ChannelLayout::Mono;
    std::atomic<ChannelId> input_buffer_idx = -1;

    // Output Info
    std::atomic<OutputDestination> output_dest = OutputDestination::Master;
    std::atomic<ChannelId> output_dest_channel = -1;
    std::atomic<ChannelLayout> output_layout = ChannelLayout::Stereo;
    std::atomic<ChannelId> output_buffer_idx = -1;
};
