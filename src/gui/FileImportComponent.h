#pragma once

#include "audio/AudioEngine.h"

#include <juce_gui_basics/juce_gui_basics.h>

class FileImportWindow: public juce::DocumentWindow {
public:
    FileImportWindow(AudioEngine& audio_engine_, FileImportWindow*& window_ptr_);
    void closeButtonPressed() override;
private:
    FileImportWindow*& window_ptr;
};

class FileImportComponent: public juce::Component, private juce::Timer {
public:
    FileImportComponent(AudioEngine& audio_engine_);
    void paint (juce::Graphics& g) override;
    void resized() override;
private:
    void reset();
    void show_status(const juce::String& message);

    void timerCallback() override;

    AudioEngine& audio_engine;

    std::unique_ptr<juce::FileChooser> audio_file_chooser;
    bool valid_file = false;
    juce::File clip_file;
    SamplePosition clip_pos;

    juce::Label file_label;
    juce::Label file_name_label;
    juce::TextButton file_select_button {"Select file"};

    juce::Label track_label;
    juce::ComboBox track_box;

    juce::Label position_label;
    juce::TextEditor position_editor;

    juce::TextButton import_button {"Import file"};

    juce::Label status_label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileImportComponent)
};
