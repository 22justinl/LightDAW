#include "gui/TrackComponent.h"

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>

// PUBLIC FUNCTIONS

TrackComponent::TrackComponent(AudioEngine& audio_engine_, TrackId track_id_)
    :   audio_engine(audio_engine_),
        track_id(track_id_),
        id_label(juce::String(), std::to_string(track_id_)),
        audio_file_label("Audio File Name", ""){

    addAndMakeVisible(id_label);

    delete_button.setButtonText("Delete");
    delete_button.onClick = [&](){
        if (on_delete) {
            on_delete();
        }
    };
    addAndMakeVisible(delete_button);

    // TODO: change button to image button
    mute_button.setButtonText("Mute");
    mute_button.setToggleable(true);
    mute_button.setToggleState(true, juce::dontSendNotification);
    mute_button.setClickingTogglesState(true);
    mute_button.onClick = [&](){
        audio_engine.track_set_mute(track_id, !audio_engine.track_is_muted(track_id));
    };
    addAndMakeVisible(mute_button);

    monitoring_button.setButtonText("Monitor");
    monitoring_button.setToggleable(true);
    monitoring_button.setToggleState(true, juce::dontSendNotification);
    monitoring_button.setClickingTogglesState(true);
    monitoring_button.onClick = [&](){
        audio_engine.track_set_monitoring(track_id, !audio_engine.track_is_monitoring(track_id));
    };
    addAndMakeVisible(monitoring_button);

    // NOTE: Temporary file import section
    audio_file_import_button.setButtonText("Import audio file");
    audio_file_import_button.onClick = [&]() { 
        audio_file_chooser = std::make_unique<juce::FileChooser>("Select an Audio File", juce::File(), "*.wav"); // TODO: other file formats
        audio_file_chooser->launchAsync(
                juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [&](const juce::FileChooser& file_chooser) {
            const auto file = file_chooser.getResult();
            if (file.existsAsFile()) {
                audio_engine.track_import_file(track_id, file);
                audio_file_label.setText(file.getFileName(), juce::dontSendNotification);
            }
        });
    };
    addAndMakeVisible(audio_file_import_button);
    addAndMakeVisible(audio_file_label);
}

void TrackComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::grey);
}

void TrackComponent::resized() {
    auto area = getLocalBounds();

    id_label.setBounds(area.removeFromLeft(75));
    area.removeFromLeft(5);
    delete_button.setBounds(area.removeFromLeft(75));
    area.removeFromLeft(5);
    mute_button.setBounds(area.removeFromLeft(75));
    area.removeFromLeft(5);
    monitoring_button.setBounds(area.removeFromLeft(75));
    area.removeFromLeft(5);
    audio_file_import_button.setBounds(area.removeFromLeft(75));
    area.removeFromLeft(5);
    audio_file_label.setBounds(area.removeFromLeft(150));
}

void TrackComponent::set_on_delete(std::function<void()> fn) {
    on_delete = fn;
}
