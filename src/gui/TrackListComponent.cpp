#include "gui/TrackListComponent.h"

// PUBLIC FUNCTIONS

TrackListComponent::TrackListComponent(AudioEngine& audio_engine_): audio_engine(audio_engine_), timeline(audio_engine_, track_height, spacing) {
    timeline_viewport.setViewedComponent(&timeline, false);
    timeline_viewport.setScrollBarsShown(false, false, false, true);
    addAndMakeVisible(timeline_viewport);
}
TrackListComponent::~TrackListComponent() {
}

void TrackListComponent::paint(juce::Graphics &g) {
    g.setColour(juce::Colours::black);
    g.fillAll();
}

void TrackListComponent::resized() {
    draw_tracks();
}

void TrackListComponent::add_track() {
    TrackId track_id = audio_engine.add_track();
    std::unique_ptr<TrackHeaderComponent> ptr = std::make_unique<TrackHeaderComponent>(audio_engine, track_id);
    addAndMakeVisible(ptr.get());
    ptr->set_on_delete([this, track_id](){ delete_track(track_id); });
    track_components[track_id] = std::move(ptr);
    timeline.add_lane(track_id);

    setSize(getLocalBounds().getWidth(), calculate_height());
}

void TrackListComponent::delete_track(TrackId track_id) {
    removeChildComponent(track_components[track_id].get());
    track_components.erase(track_id);
    timeline.delete_lane(track_id);
    audio_engine.erase_track(track_id);

    setSize(getLocalBounds().getWidth(), calculate_height());
}


int TrackListComponent::calculate_height() const {
    if (track_components.empty()) { return 0; }
    return 2*spacing + static_cast<int>(track_components.size()) * (track_height + spacing) - spacing;
}

void TrackListComponent::draw_tracks() {

    auto timeline_box = getLocalBounds();
    auto header_col = timeline_box.removeFromLeft(header_width);
    header_col.removeFromLeft(spacing);
    header_col.removeFromRight(spacing);
    header_col.removeFromTop(spacing);
    header_col.removeFromBottom(spacing);

    const auto& track_ids = audio_engine.get_track_ids();
    for (TrackId track_id : track_ids) {
        track_components[track_id]->setBounds(header_col.removeFromTop(track_height));
        if (header_col.isEmpty()) {
            break;
        }
        header_col.removeFromTop(spacing);
    }

    timeline_viewport.setBounds(timeline_box);
    timeline.setSize(1000, timeline_viewport.getMaximumVisibleHeight());
}
