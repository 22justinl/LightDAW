#include "audio/DAWIOCallback.h"

#include "audio/AudioEngine.h"

#include <juce_audio_devices/juce_audio_devices.h>

// PUBLIC FUNCTIONS

DAWIOCallback::DAWIOCallback(AudioEngine& audio_engine_): audio_engine(audio_engine_) { }

void DAWIOCallback::audioDeviceAboutToStart(juce::AudioIODevice* device) {
    audio_engine.get_track_mixer_source().prepareToPlay(device->getCurrentBufferSizeSamples(), device->getCurrentSampleRate());
}

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

    for (int channel = 0; channel < numOutputChannels; ++channel) {
        juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
    }

    juce::AudioBuffer<float> buffer(outputChannelData, numOutputChannels, numSamples);
    juce::AudioSourceChannelInfo info(&buffer, 0, numSamples);

    audio_engine.set_input_view_ptr(inputChannelData);
    audio_engine.get_track_mixer_source().getNextAudioBlock(info);
    audio_engine.set_input_view_ptr(nullptr);

    if (audio_engine.is_playing() && audio_engine.is_playback_ready()) {
        audio_engine.advance_playhead(numSamples);
        audio_engine.check_threshold_and_notify();
    }


    // TODO: remove after checking monitoring during playback (remember to commit)
    // if (audio_engine.is_playing()) {
    //     if (audio_engine.is_playback_ready()) {
    //         juce::AudioBuffer<float> buffer(
    //                 outputChannelData,
    //                 numOutputChannels,
    //                 numSamples);
    //
    //         juce::AudioSourceChannelInfo info(&buffer, 0, numSamples);
    //         audio_engine.get_track_mixer_source().getNextAudioBlock(info);
    //     }
    //     audio_engine.advance_playhead(numSamples);
    //     audio_engine.check_threshold_and_notify();
    // }
}
