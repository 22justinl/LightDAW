#include "gui/TimelineComponent.h"

TimelineLaneComponent::TimelineLaneComponent(Track& track_, int& track_height_, double& pixels_per_sample_)
    : track(track_), track_height(track_height_), pixels_per_sample(pixels_per_sample_)
{
    for (Clip& clip : track.clips) {
        clip_components.emplace(clip.id, std::make_unique<ClipComponent>(track, clip.id));
    }
    track.addChangeListener(this);
}
void TimelineLaneComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::red);
}
void TimelineLaneComponent::resized() {
    update_all_clip_bounds();
}

void TimelineLaneComponent::changeListenerCallback(juce::ChangeBroadcaster*) {
    while (!track.clip_change_queue.empty()) {
        const auto& change = track.clip_change_queue.front();
        switch (change.type) {
            case ClipChangeType::Add:
                add_clip(change.id);
                break;
            case ClipChangeType::Delete:
                delete_clip(change.id);
                break;
            case ClipChangeType::Edit:
                update_clip(change.id);
                break;
            case ClipChangeType::None:
            default:
                break;
        }
        track.clip_change_queue.pop();
    }
}

void TimelineLaneComponent::add_clip(ClipId id) {
    clip_components.emplace(id, std::make_unique<ClipComponent>(track, id));
    addAndMakeVisible(clip_components[id].get());
    update_clip_bounds(id);
}

void TimelineLaneComponent::delete_clip(ClipId id) {
    removeChildComponent(clip_components[id].get());
    clip_components.erase(id);
}

void TimelineLaneComponent::update_clip(ClipId id) {
    update_clip_bounds(id);
}

void TimelineLaneComponent::update_clip_bounds(ClipId id) {
    const Clip& clip = *track.get_clip_by_id(id);
    auto area = getLocalBounds();
    area = area.removeFromLeft(static_cast<int>(static_cast<double>(clip.end_pos) * pixels_per_sample));
    area.removeFromLeft(static_cast<int>(static_cast<double>(clip.pos) * pixels_per_sample));
    clip_components[id]->setBounds(area);
}

void TimelineLaneComponent::update_all_clip_bounds() {
    for (const auto& p : clip_components) {
        update_clip_bounds(p.first);
    }
}

TimelineComponent::TimelineComponent(AudioEngine& audio_engine_, int& track_height_): audio_engine(audio_engine_), track_height(track_height_) { }
void TimelineComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

void TimelineComponent::resized() {
    auto area = getLocalBounds();

    const auto& track_ids = audio_engine.get_track_ids();
    for (TrackId track_id : track_ids) {
        track_lanes[track_id]->setBounds(area.removeFromTop(track_height));
        if (area.isEmpty()) {
            break;
        }
    }
}

void TimelineComponent::add_lane(TrackId id) {
    track_lanes.emplace(id, std::make_unique<TimelineLaneComponent>(*audio_engine.get_tracks()[id], track_height, pixels_per_sample));
    addAndMakeVisible(track_lanes[id].get());
}
void TimelineComponent::delete_lane(TrackId id) {
    removeChildComponent(track_lanes[id].get());
    track_lanes.erase(id);
}

