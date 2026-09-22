#pragma once

#include "audio/AudioEngine.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>

class TrackComponent: public juce::Component {
public:
    TrackComponent(AudioEngine& audio_engine_, TrackId track_id_);
    void paint(juce::Graphics& g) override;
    void resized() override;

    void set_on_delete(std::function<void()> fn);
private:
    AudioEngine& audio_engine;

    TrackId track_id;
    std::unique_ptr<juce::FileChooser> audio_file_chooser;
    std::function<void()> on_delete;

    juce::Label id_label;
    juce::TextButton delete_button;
    juce::TextButton mute_button;
    juce::TextButton monitoring_button;
    juce::TextButton audio_file_import_button;
    juce::Label audio_file_label;
};
