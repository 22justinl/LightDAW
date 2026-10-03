#pragma once

#include "audio/AudioEngine.h"

#include "gui/ClipComponent.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <unordered_map>

class TimelineLaneComponent: public juce::Component, private juce::ChangeListener {
public:
    TimelineLaneComponent(Track& track_, int& track_height_, double& pixels_per_sample);
    void paint(juce::Graphics &g) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    void build_clips();
    void add_clip(ClipId id);
    void delete_clip(ClipId id);
    void update_clip(ClipId id);
    void update_all_clip_bounds();
private:
    void update_clip_bounds(ClipId id);

    std::unordered_map<size_t, std::unique_ptr<ClipComponent>> clip_components;

    Track& track;
    int& track_height;
    double& pixels_per_sample;
};

class TimelineComponent: public juce::Component {
public:
    TimelineComponent(AudioEngine& audio_engine_, int& track_height_);
    void paint(juce::Graphics &g) override;
    void resized() override;

    void add_lane(TrackId id);
    void delete_lane(TrackId id);
private:
    AudioEngine& audio_engine;
    std::unordered_map<TrackId, std::unique_ptr<TimelineLaneComponent>> track_lanes;
    int& track_height;
    double pixels_per_sample = 800.0/(44100*60);
    SamplePosition timeline_sample_length = 44100 * 60;
};
