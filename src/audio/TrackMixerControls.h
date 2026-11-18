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
    float get_gain() const;
    void set_gain(float new_gain_db);

    float get_pan() const;
    void set_pan(float new_pan);

    bool is_muted() const;
    void set_muted(bool new_muted);

    bool is_soloed() const;
    void set_soloed(bool new_soloed);

    bool is_recording() const;
    void set_recording(bool new_recording);

    bool is_monitoring() const;
    void set_monitoring(bool new_monitoring);

    OutputDestination get_output_dest() const;
    ChannelId get_output_channel() const;
    ChannelLayout get_output_layout() const;

    std::atomic<float> gain_db = 0;
    std::atomic<float> pan = 0;

    std::atomic_bool muted = false;
    std::atomic_bool soloed = false;
    std::atomic_bool recording = false;
    std::atomic_bool monitoring = false;


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
