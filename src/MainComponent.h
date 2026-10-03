#pragma once

#include "audio/AudioEngine.h"

#include "gui/AudioSettingsComponent.h"
#include "gui/FileImportComponent.h"
#include "gui/TrackListComponent.h"
#include "gui/TransportComponent.h"

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class MainComponent final : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
private:
    AudioEngine audio_engine;

    // GUI Components
    // TODO: use MenuBarComponent
    juce::TextButton settings_button {"Audio Settings"};
    juce::TextButton add_track_button {"Add Track"};
    juce::TextButton import_file_button {"Import File"};
    // TODO: delete clip button, also rename import file to add clip?
    TransportComponent transport_component;

    juce::Viewport track_list_viewport;

    TrackListComponent track_list;
    AudioSettingsWindow* audio_settings_window_ptr = nullptr;
    FileImportWindow* import_file_window_ptr = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)

};
