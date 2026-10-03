#include "MainComponent.h"

#include "gui/AudioSettingsComponent.h"
#include "gui/FileImportComponent.h"
#include "gui/TrackListComponent.h"
#include "gui/TransportComponent.h"

MainComponent::MainComponent(): transport_component(audio_engine), track_list(audio_engine) {
    setSize (1200, 800);

    settings_button.onClick = [&](){ if (!audio_settings_window_ptr) {new AudioSettingsWindow(audio_engine, audio_settings_window_ptr);} };

    add_track_button.onClick = [&](){ track_list.add_track(); };

    import_file_button.onClick = [&](){ if (!import_file_window_ptr) {new FileImportWindow(audio_engine, import_file_window_ptr);} };

    track_list.add_track();
    track_list_viewport.setViewedComponent(&track_list, false);
    track_list_viewport.setScrollBarsShown(false, false, true, false);

    addAndMakeVisible(settings_button);
    addAndMakeVisible(add_track_button);
    addAndMakeVisible(import_file_button);
    addAndMakeVisible(track_list_viewport);
    addAndMakeVisible(transport_component);
}

MainComponent::~MainComponent() {
    if (audio_settings_window_ptr) {
        delete audio_settings_window_ptr;
    }
    if (import_file_window_ptr) {
        delete import_file_window_ptr;
    }
}

void MainComponent::paint (juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
    auto area = getLocalBounds().reduced(10);

    auto top_area = area.removeFromTop(75);
    settings_button.setBounds(top_area.removeFromLeft(75));
    add_track_button.setBounds(top_area.removeFromLeft(75));
    import_file_button.setBounds(top_area.removeFromLeft(75));
    transport_component.setBounds(top_area);

    area.removeFromTop(10);

    track_list_viewport.setBounds(area);
    track_list.setSize(track_list_viewport.getMaximumVisibleWidth(), track_list.calculate_height());
}
