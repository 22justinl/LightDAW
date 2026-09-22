#include "MainComponent.h"

#include "gui/TrackListComponent.h"

MainComponent::MainComponent()
    :   track_list(audio_engine),
        audio_settings(audio_engine) {
    setSize (1200, 800);

    settings_button.setButtonText("Audio Settings");
    settings_button.onClick = [&](){ audio_settings.setVisible(!audio_settings.isVisible()); };
    addAndMakeVisible(settings_button);

    add_track_button.onClick = [&](){ track_list.add_track(); };
    addAndMakeVisible(add_track_button);

    track_list.add_track();
    track_list_viewport.setViewedComponent(&track_list);
    track_list_viewport.setScrollBarsShown(false, false, true, false);
    addAndMakeVisible(track_list_viewport);

    addChildComponent(audio_settings);
}


void MainComponent::paint (juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
    auto area = getLocalBounds().reduced(10);

    audio_settings.setCentrePosition(area.getCentreX(), area.getCentreY());

    auto top_area = area.removeFromTop(75);
    settings_button.setBounds(top_area.removeFromLeft(75));
    add_track_button.setBounds(top_area.removeFromLeft(75));

    area.removeFromTop(10);

    track_list_viewport.setBounds(area);
    track_list.setSize(track_list_viewport.getMaximumVisibleWidth(), track_list.calculate_height());
}
