#include "gui/AudioSettingsComponent.h"

#include <juce_audio_utils/juce_audio_utils.h>

// PUBLIC FUNCTIONS

AudioSettingsComponent::AudioSettingsComponent(AudioEngine& audio_engine_)
    :   audio_engine(audio_engine_),
        device_selector(
            audio_engine_.device_manager,
            0, 2, 0, 2, true, false, true, false) {
    setSize(600, 400);
    toFront(true);

    addAndMakeVisible(device_selector);
}

void AudioSettingsComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::grey);
}

void AudioSettingsComponent::resized() {
    auto area = getLocalBounds();

    device_selector.setBounds(area.getX(), area.getY()+10, area.getWidth(), area.getHeight()-10);
}
