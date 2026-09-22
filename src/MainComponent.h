#pragma once

#include "audio/AudioEngine.h"

#include "gui/AudioSettingsComponent.h"
#include "gui/TrackListComponent.h"

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class MainComponent final : public juce::Component
{
public:
    MainComponent();

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AudioEngine audio_engine;

    // GUI Components
    juce::TextButton settings_button;
    juce::TextButton add_track_button;

    juce::Viewport track_list_viewport;
    TrackListComponent track_list;

    AudioSettingsComponent audio_settings;
    int track_height = 50;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)

};
