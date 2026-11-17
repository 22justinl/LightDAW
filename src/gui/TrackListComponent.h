#pragma once

#include "audio/AudioEngine.h"

#include "gui/TimelineComponent.h"
#include "gui/TrackHeaderComponent.h"

#include <juce_gui_basics/juce_gui_basics.h>

class TrackListComponent: public juce::Component {
public:
    TrackListComponent(AudioEngine& audio_engine_);
    ~TrackListComponent() override;
    void paint(juce::Graphics& g) override;

    void resized() override;

    void add_track();
    void delete_track(TrackId track_id);

    int calculate_height() const;
private:
    void draw_tracks();

    AudioEngine& audio_engine;

    juce::Viewport timeline_viewport;

    std::unordered_map<TrackId, std::unique_ptr<TrackHeaderComponent>> track_components;
    // 20+4+4+5+4+4*34+3*4+8+4*34+3*4+8+4*34+3*4
    int header_width = 497;
    int track_height = 116;
    int spacing = 2;

    TimelineComponent timeline;
};
