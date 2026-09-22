#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

class AudioEngine;

class DAWIOCallback: public juce::AudioIODeviceCallback {
public:
    DAWIOCallback(AudioEngine& audio_engine_);
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    void audioDeviceError(const juce::String& errorMessage) override;
    void audioDeviceIOCallbackWithContext(
            const float* const* inputChannelData,
            int numInputChannels,
            float* const* outputChannelData,
            int numOutputChannels,
            int numSamples,
            const juce::AudioIODeviceCallbackContext& context) override;
private:
    AudioEngine& audio_engine;
};
