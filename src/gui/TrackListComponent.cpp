#include "gui/TrackListComponent.h"

// PUBLIC FUNCTIONS

TrackListComponent::TrackListComponent(AudioEngine& audio_engine_): audio_engine(audio_engine_) {}

void TrackListComponent::paint(juce::Graphics &g) {
    g.setColour(juce::Colours::grey);
}

void TrackListComponent::resized() {
    auto area = getLocalBounds();

    for (auto* track : tracks) {
        track->setBounds(area.removeFromTop(track_height));
        if (area.isEmpty()) {
            break;
        }
        area.removeFromTop(spacing);
    }
}

void TrackListComponent::add_track() {
    TrackId track_id = audio_engine.add_track();
    TrackComponent* ptr = tracks.add(new TrackComponent(audio_engine, track_id));
    id_to_track_ptr[track_id] = ptr;
    ptr->set_on_delete([this, track_id](){ delete_track(track_id); });
    addAndMakeVisible(ptr);

    setSize(getLocalBounds().getWidth(), calculate_height());

    resized();
}

void TrackListComponent::delete_track(TrackId track_id) {
    TrackComponent* ptr = id_to_track_ptr[track_id];
    id_to_track_ptr.erase(track_id);
    tracks.removeObject(ptr);
    audio_engine.erase_track(track_id);

    resized();
}

int TrackListComponent::calculate_height() const {
    if (tracks.isEmpty()) { return 0; }
    return track_height * tracks.size() + spacing * (tracks.size()-1);
}
