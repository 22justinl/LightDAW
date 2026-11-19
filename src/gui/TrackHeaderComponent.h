#pragma once

#include "audio/AudioEngine.h"
// #include "gui/LevelMeterComponent.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>

constexpr int spacing = 4;
constexpr int border_width = 2;
constexpr int item_width = 34;
constexpr int item_height = 24;

struct TrackHeaderLayout {
    juce::Rectangle<int> number;

    juce::Rectangle<int> number_name_border;

    juce::Rectangle<int> name;

    juce::Rectangle<int> name_channel_border;

    // channel selection dropdowns
    juce::Rectangle<int> input_src;
    juce::Rectangle<int> input_channel;
    juce::Rectangle<int> output_dst;
    juce::Rectangle<int> output_channel;

    juce::Rectangle<int> channel_mixer_border;

    // mixer controls
    juce::Rectangle<int> mute;
    juce::Rectangle<int> solo;
    juce::Rectangle<int> recording;
    juce::Rectangle<int> monitoring;

    juce::Rectangle<int> volume;
    juce::Rectangle<int> pan;

    juce::Rectangle<int> volume_meter;
};

class TrackHeaderComponent: public juce::Component, public juce::ChangeListener {
public:
    TrackHeaderComponent(AudioEngine& audio_engine_, TrackId track_id_);
    ~TrackHeaderComponent() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    void set_on_delete(std::function<void()> fn);
private:
    TrackHeaderLayout calculate_layout() const;

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
