#pragma once

#include "audio/AudioEngine.h"

#include "gui/TrackComponent.h"

#include <juce_gui_basics/juce_gui_basics.h>

class TrackListComponent: public juce::Component {
public:
    TrackListComponent(AudioEngine& audio_engine_);
    void paint(juce::Graphics& g) override;

    void resized() override;

    void add_track();
    void delete_track(TrackId track_id);

    int calculate_height() const;
private:
    AudioEngine& audio_engine;

    juce::OwnedArray<TrackComponent> tracks;
    std::unordered_map<TrackId, TrackComponent*> id_to_track_ptr;

    int track_height = 50;
    int spacing = 5;
};
