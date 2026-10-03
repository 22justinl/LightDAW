#include "gui/AudioSettingsComponent.h"

#include <juce_audio_utils/juce_audio_utils.h>

// PUBLIC FUNCTIONS

AudioSettingsWindow::AudioSettingsWindow(AudioEngine& audio_engine_, AudioSettingsWindow*& window_ptr_)
    :   juce::DocumentWindow("Audio Device Settings", juce::Colours::grey, juce::DocumentWindow::closeButton), window_ptr(window_ptr_)
{
    setContentOwned(new AudioSettingsComponent(audio_engine_), true);
    centreWithSize(600, 400);
    setResizable(false, false);
    setVisible(true);
    window_ptr = this;
}
void AudioSettingsWindow::closeButtonPressed() {
    window_ptr = nullptr;
    delete this;
}

AudioSettingsComponent::AudioSettingsComponent(AudioEngine& audio_engine_)
    :   audio_engine(audio_engine_),
        device_selector(
            audio_engine_.device_manager,
            0, 2, 0, 2, true, false, true, false) {
    setSize(600, 400);

    addAndMakeVisible(device_selector);
}

void AudioSettingsComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::grey);
}

void AudioSettingsComponent::resized() {
    auto area = getLocalBounds();

    device_selector.setBounds(area.getX(), area.getY()+10, area.getWidth(), area.getHeight()-10);
}
