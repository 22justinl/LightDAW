#include "gui/TrackHeaderComponent.h"

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

// PUBLIC FUNCTIONS

TrackHeaderComponent::TrackHeaderComponent(AudioEngine& audio_engine_, TrackId track_id_)
    :   audio_engine(audio_engine_),
        track_id(track_id_),
        id_label(juce::String(), std::to_string(track_id_)) {

    addAndMakeVisible(id_label);

    delete_button.onClick = [&](){
        if (on_delete) {
            on_delete();
        }
    };
    addAndMakeVisible(delete_button);

    // TODO: change button to image button
    mute_button.setToggleable(true);
    mute_button.setToggleState(true, juce::dontSendNotification);
    mute_button.setClickingTogglesState(true);
    mute_button.onClick = [&](){
        audio_engine.track_set_mute(track_id, !audio_engine.track_is_muted(track_id));
    };
    addAndMakeVisible(mute_button);

    monitoring_button.setToggleable(true);
    monitoring_button.setToggleState(true, juce::dontSendNotification);
    monitoring_button.setClickingTogglesState(true);
    monitoring_button.onClick = [&](){
        audio_engine.track_set_monitoring(track_id, !audio_engine.track_is_monitoring(track_id));
    };
    addAndMakeVisible(monitoring_button);
}

void TrackHeaderComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::grey);
}

void TrackHeaderComponent::resized() {
    auto area = getLocalBounds();

    id_label.setBounds(area.removeFromLeft(75));
    area.removeFromLeft(5);
    delete_button.setBounds(area.removeFromLeft(75));
    area.removeFromLeft(5);
    mute_button.setBounds(area.removeFromLeft(75));
    area.removeFromLeft(5);
    monitoring_button.setBounds(area.removeFromLeft(75));
}

void TrackHeaderComponent::set_on_delete(std::function<void()> fn) {
    on_delete = fn;
}
