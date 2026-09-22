#include "audio/DAWIOCallback.h"

#include "audio/AudioEngine.h"

#include <juce_audio_devices/juce_audio_devices.h>

// #include <algorithm>

// PUBLIC FUNCTIONS

DAWIOCallback::DAWIOCallback(AudioEngine& audio_engine_): audio_engine(audio_engine_) { }

void DAWIOCallback::audioDeviceAboutToStart(juce::AudioIODevice*) { }

void DAWIOCallback::audioDeviceStopped() { }

void DAWIOCallback::audioDeviceError(const juce::String& errorMessage) {
    DBG("DAWIOCallback Error: " + errorMessage);
}

void DAWIOCallback::audioDeviceIOCallbackWithContext(
        const float* const* inputChannelData,
        int numInputChannels,
        float* const* outputChannelData,
        int numOutputChannels,
        int numSamples,
        const juce::AudioIODeviceCallbackContext&) {
    juce::ignoreUnused(numInputChannels);

    const std::pair<ChannelId, ChannelId> output_channels = audio_engine.output_channels();
    const ChannelId left_channel = output_channels.first;
    const ChannelId right_channel = output_channels.second;
    for (int channel = 0; channel < numOutputChannels; ++channel) {
        juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
    }
    if (left_channel == -1) { return; }

    for (const auto& p : audio_engine.get_tracks()) {
        Track& track = *p.second;
        if (track.is_muted()) { continue; }
        if (track.is_monitoring()) {
            juce::FloatVectorOperations::add(outputChannelData[left_channel], inputChannelData[track.get_buffer_channel()], numSamples);
            if (right_channel != -1) {
                juce::FloatVectorOperations::add(outputChannelData[right_channel], inputChannelData[track.get_buffer_channel()], numSamples);
            }
        }

        // TODO: add track audio
    }
    // if (input_muted) {
    //     for (int channel = 0; channel < numOutputChannels; ++channel) {
    //         juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
    //     }
    //     return;
    // }
    //
    // if (numInputChannels == 1) {
    //     for (int channel = 0; channel < numOutputChannels; ++channel) {
    //         juce::FloatVectorOperations::copyWithMultiply(outputChannelData[channel], inputChannelData[0], input_gain, numSamples);
    //     }
    // } else {
    //     for (int channel = 0; channel < std::min(numInputChannels, numOutputChannels); ++channel) {
    //         juce::FloatVectorOperations::copyWithMultiply(outputChannelData[channel], inputChannelData[channel], input_gain, numSamples);
    //     }
    // }
}
