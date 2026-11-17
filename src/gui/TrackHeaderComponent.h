#pragma once

#include "audio/AudioEngine.h"
// #include "gui/LevelMeterComponent.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>

// class TrackHeaderComponent: public juce::Component {
// public:
//     TrackHeaderComponent(AudioEngine& audio_engine_, TrackId track_id_);
//     void paint(juce::Graphics& g) override;
//     void resized() override;
//
//     void set_on_delete(std::function<void()> fn);
// private:
//     AudioEngine& audio_engine;
//
//     TrackId track_id;
//     std::unique_ptr<juce::FileChooser> audio_file_chooser;
//     std::function<void()> on_delete;
//
//     juce::Label id_label;
//     juce::TextButton delete_button {"Delete"};
//     juce::TextButton mute_button {"Mute"};
//     juce::TextButton monitoring_button {"Monitor"};
//
// };

constexpr int spacing = 4;
constexpr int border_width = 2;
constexpr int item_width = 34;
constexpr int item_height = 24;

class TrackHeaderComponent: public juce::Component, public juce::ChangeListener {
public:
    TrackHeaderComponent(AudioEngine& audio_engine_, TrackId track_id_);
    ~TrackHeaderComponent() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    void set_on_delete(std::function<void()> fn);
private:
    void update_input_channel_dropdown();
    void update_output_channel_dropdown();
    void update_input_channel();
    void update_output_channel();

    AudioEngine& audio_engine;

    TrackId track_id;
    std::unique_ptr<juce::FileChooser> audio_file_chooser;
    std::function<void()> on_delete;

    juce::LookAndFeel_V4 look_and_feel;

    juce::Label track_number_label;
    juce::Label track_name_label;
    juce::TextEditor name_editor;

    juce::ComboBox input_src_dropdown;
    juce::ComboBox input_channel_dropdown;
    juce::ComboBox output_dst_dropdown;
    juce::ComboBox output_channel_dropdown;

    juce::TextButton mute_button {"M"};
    juce::TextButton solo_button {"S"};
    juce::TextButton recording_button {"R"};
    juce::TextButton monitoring_button {"I"};

    juce::Slider volume_slider;
    juce::Slider pan_slider;

    // LevelMeterComponent left_level_meter_component;
    // LevelMeterComponent right_level_meter_component;
};
