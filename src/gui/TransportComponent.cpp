#include "gui/TransportComponent.h"

TransportComponent::TransportComponent(AudioEngine& audio_engine_): audio_engine(audio_engine_) {
    addAndMakeVisible(play_button);
    addAndMakeVisible(stop_button);
    addAndMakeVisible(position_label);
    addAndMakeVisible(wake_button);

    play_button.onClick = [&] {
        if (audio_engine.is_playing()) {
            audio_engine.stop();
        } else {
            audio_engine.play();
        }
        update_play_button();
    };

    stop_button.onClick = [&] {
        audio_engine.stop();
        update_play_button();
    };

    wake_button.onClick = [&] {
        audio_engine.notify_cv();
    };

    position_label.setText("0", juce::dontSendNotification);
    position_label.setJustificationType(juce::Justification::centred);

    startTimerHz(10);
}

void TransportComponent::resized()
{
    auto area = getLocalBounds().reduced(5);

    play_button.setBounds(area.removeFromLeft(80));
    area.removeFromLeft(5);

    stop_button.setBounds(area.removeFromLeft(80));
    area.removeFromLeft(10);

    position_label.setBounds(area.removeFromLeft(100));
    area.removeFromLeft(10);

    wake_button.setBounds(area.removeFromLeft(100));
}

void TransportComponent::timerCallback()
{
    position_label.setText(
        juce::String(audio_engine.get_position()),
        juce::dontSendNotification);

    update_play_button();
}

void TransportComponent::update_play_button()
{
    play_button.setButtonText(
        audio_engine.is_playing() ? "Pause" : "Play");
}
