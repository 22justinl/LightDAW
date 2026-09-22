#pragma once

#include "audio/AudioEngine.h"

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_utils/juce_audio_utils.h>

class AudioSettingsComponent final: public juce::Component {
public:
    AudioSettingsComponent(AudioEngine& audio_engine_);
    void paint (juce::Graphics& g) override;
    void resized() override;
private:
    AudioEngine& audio_engine;

    juce::AudioDeviceSelectorComponent device_selector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioSettingsComponent)
};
