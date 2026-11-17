#include "gui/TrackHeaderComponent.h"
#include "juce_graphics/juce_graphics.h"

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

// PUBLIC FUNCTIONS

// TrackHeaderComponent::TrackHeaderComponent(AudioEngine& audio_engine_, TrackId track_id_)
//     :   audio_engine(audio_engine_),
//         track_id(track_id_),
//         id_label(juce::String(), std::to_string(track_id_)) {
//
//     addAndMakeVisible(id_label);
//
//     delete_button.onClick = [&](){
//         if (on_delete) {
//             on_delete();
//         }
//     };
//     addAndMakeVisible(delete_button);
//
//     // TODO: change button to image button
//     mute_button.setToggleable(true);
//     mute_button.setToggleState(true, juce::dontSendNotification);
//     mute_button.setClickingTogglesState(true);
//     mute_button.onClick = [&](){
//         audio_engine.track_set_mute(track_id, !audio_engine.track_is_muted(track_id));
//     };
//     addAndMakeVisible(mute_button);
//
//     monitoring_button.setToggleable(true);
//     monitoring_button.setToggleState(true, juce::dontSendNotification);
//     monitoring_button.setClickingTogglesState(true);
//     monitoring_button.onClick = [&](){
//         audio_engine.track_set_monitoring(track_id, !audio_engine.track_is_monitoring(track_id));
//     };
//     addAndMakeVisible(monitoring_button);
// }
//
// void TrackHeaderComponent::paint(juce::Graphics& g) {
//     g.fillAll(juce::Colours::grey);
// }
//
// void TrackHeaderComponent::resized() {
//     auto area = getLocalBounds();
//
//     id_label.setBounds(area.removeFromLeft(75));
//     area.removeFromLeft(5);
//     delete_button.setBounds(area.removeFromLeft(75));
//     area.removeFromLeft(5);
//     mute_button.setBounds(area.removeFromLeft(75));
//     area.removeFromLeft(5);
//     monitoring_button.setBounds(area.removeFromLeft(75));
// }
//
// void TrackHeaderComponent::set_on_delete(std::function<void()> fn) {
//     on_delete = fn;
// }

TrackHeaderComponent::TrackHeaderComponent(AudioEngine& audio_engine_, TrackId track_id_)
    :   audio_engine(audio_engine_),
        track_id(track_id_) {
    audio_engine.get_channel_manager().addChangeListener(this);

    track_number_label.setText(juce::String(track_id), juce::dontSendNotification);
    track_name_label.setText(audio_engine.get_tracks()[track_id]->get_name(), juce::dontSendNotification);
    track_name_label.setJustificationType(juce::Justification::topLeft);

    mute_button.setToggleable(true);
    mute_button.setToggleState(true, juce::dontSendNotification);
    mute_button.setClickingTogglesState(true);
    mute_button.onClick = [&](){
        audio_engine.track_set_mute(track_id, !audio_engine.track_is_muted(track_id));
    };
    // TODO: Solo button
    // TODO: Recording button
    monitoring_button.setToggleable(true);
    monitoring_button.setToggleState(true, juce::dontSendNotification);
    monitoring_button.setClickingTogglesState(true);
    monitoring_button.onClick = [&](){
        audio_engine.track_set_monitoring(track_id, !audio_engine.track_is_monitoring(track_id));
    };

    volume_slider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    volume_slider.setScrollWheelEnabled(false);
    volume_slider.setNumDecimalPlacesToDisplay(2);

    pan_slider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    pan_slider.setScrollWheelEnabled(false);
    pan_slider.setNumDecimalPlacesToDisplay(2);


    track_number_label.setLookAndFeel(&look_and_feel);
    track_name_label.setLookAndFeel(&look_and_feel);

    input_src_dropdown.setLookAndFeel(&look_and_feel);
    input_src_dropdown.addItem("Ext. In", static_cast<int>(InputSource::External));
    input_src_dropdown.setSelectedId(static_cast<int>(InputSource::External), juce::dontSendNotification);
    input_src_dropdown.onChange = [&]() { update_input_channel_dropdown(); };

    input_channel_dropdown.setLookAndFeel(&look_and_feel);
    input_channel_dropdown.onChange = [&]() { update_input_channel(); };

    output_dst_dropdown.setLookAndFeel(&look_and_feel);
    output_dst_dropdown.addItem("Master", static_cast<int>(OutputDestination::Master));
    output_dst_dropdown.addItem("Ext. Out", static_cast<int>(OutputDestination::External));
    output_dst_dropdown.setSelectedId(static_cast<int>(OutputDestination::Master), juce::dontSendNotification);
    output_dst_dropdown.onChange = [&]() { update_output_channel_dropdown(); };

    output_channel_dropdown.setLookAndFeel(&look_and_feel);
    output_channel_dropdown.onChange = [&]() { update_output_channel(); };

    mute_button.setLookAndFeel(&look_and_feel);
    solo_button.setLookAndFeel(&look_and_feel);
    recording_button.setLookAndFeel(&look_and_feel);
    monitoring_button.setLookAndFeel(&look_and_feel);

    volume_slider.setLookAndFeel(&look_and_feel);
    pan_slider.setLookAndFeel(&look_and_feel);

    addAndMakeVisible(track_number_label);
    addAndMakeVisible(track_name_label);

    addAndMakeVisible(input_src_dropdown);
    addAndMakeVisible(input_channel_dropdown);
    addAndMakeVisible(output_dst_dropdown);
    addAndMakeVisible(output_channel_dropdown);

    addAndMakeVisible(mute_button);
    addAndMakeVisible(solo_button);
    addAndMakeVisible(recording_button);
    addAndMakeVisible(monitoring_button);

    addAndMakeVisible(volume_slider);
    addAndMakeVisible(pan_slider);

    // addAndMakeVisible(left_level_meter_component);
    // addAndMakeVisible(right_level_meter_component);
}

TrackHeaderComponent::~TrackHeaderComponent() {
    audio_engine.get_channel_manager().removeChangeListener(this);

    track_number_label.setLookAndFeel(nullptr);
    track_name_label.setLookAndFeel(nullptr);

    input_src_dropdown.setLookAndFeel(nullptr);
    input_channel_dropdown.setLookAndFeel(nullptr);
    output_dst_dropdown.setLookAndFeel(nullptr);
    output_channel_dropdown.setLookAndFeel(nullptr);

    mute_button.setLookAndFeel(nullptr);
    solo_button.setLookAndFeel(nullptr);
    recording_button.setLookAndFeel(nullptr);
    monitoring_button.setLookAndFeel(nullptr);

    volume_slider.setLookAndFeel(nullptr);
    pan_slider.setLookAndFeel(nullptr);
}

void TrackHeaderComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::grey);
    auto area = getLocalBounds();

    auto number_box = area.removeFromLeft(36);
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(number_box);

    g.setColour(juce::Colours::black);
    g.fillRect(area.removeFromLeft(border_width));

    area.removeFromRight(spacing);
    area.removeFromRight(5);
    area.removeFromRight(spacing);
    area.removeFromRight(4*item_width + 3*spacing);
    area.removeFromRight(spacing);

    g.setColour(juce::Colours::black);
    g.fillRect(area.removeFromRight(border_width));

    area.removeFromRight(spacing);
    area.removeFromRight(4*item_width + 3*spacing);
    area.removeFromRight(spacing);

    g.setColour(juce::Colours::black);
    g.fillRect(area.removeFromRight(border_width));
}

void TrackHeaderComponent::resized() {
    auto area = getLocalBounds();

    auto number_box = area.removeFromLeft(36);

    area.removeFromLeft(border_width);
    area.removeFromLeft(spacing);
    area.removeFromTop(spacing);
    area.removeFromBottom(spacing);

    area.removeFromRight(spacing);
    auto volume_box = area.removeFromRight(5);
    area.removeFromRight(spacing);
    auto mixer_control_box = area.removeFromRight(4*item_width + 3*spacing);
    area.removeFromRight(spacing);

    area.removeFromRight(border_width);

    area.removeFromRight(spacing);
    auto channel_selection_box = area.removeFromRight(4*item_width + 3*spacing);
    area.removeFromRight(spacing);

    area.removeFromRight(border_width);


    auto msri_box = mixer_control_box.removeFromTop(item_height);
    mixer_control_box.removeFromTop(spacing);
    auto volume_pan_box = mixer_control_box.removeFromTop(item_height);

    track_number_label.setBounds(number_box);
    track_name_label.setBounds(area);

    input_src_dropdown.setBounds(channel_selection_box.removeFromTop(item_height));
    channel_selection_box.removeFromTop(spacing);
    input_channel_dropdown.setBounds(channel_selection_box.removeFromTop(item_height));
    channel_selection_box.removeFromTop(spacing);
    output_dst_dropdown.setBounds(channel_selection_box.removeFromTop(item_height));
    channel_selection_box.removeFromTop(spacing);
    output_channel_dropdown.setBounds(channel_selection_box.removeFromTop(item_height));

    mute_button.setBounds(msri_box.removeFromLeft(item_width));
    msri_box.removeFromLeft(spacing);
    solo_button.setBounds(msri_box.removeFromLeft(item_width));
    msri_box.removeFromLeft(spacing);
    recording_button.setBounds(msri_box.removeFromLeft(item_width));
    msri_box.removeFromLeft(spacing);
    monitoring_button.setBounds(msri_box.removeFromLeft(item_width));

    volume_slider.setBounds(volume_pan_box.removeFromLeft(2.5*item_width + 1*spacing));
    volume_pan_box.removeFromLeft(spacing);
    pan_slider.setBounds(volume_pan_box.removeFromLeft(1.5*item_width+spacing));
}

void TrackHeaderComponent::changeListenerCallback(juce::ChangeBroadcaster*) {
    update_input_channel_dropdown();
    update_output_channel_dropdown();
}

void TrackHeaderComponent::update_input_channel_dropdown() {
    input_channel_dropdown.clear(juce::dontSendNotification);
    if (input_src_dropdown.getSelectedId() != static_cast<int>(InputSource::External)) {
        input_channel_dropdown.setEnabled(false);
        return;
    }

    const auto& input_channels = audio_engine.get_channel_manager().get_input_channel_strings();

    input_channel_dropdown.setEnabled(true);

    input_channel_dropdown.addItemList(input_channels.at(ChannelLayout::Mono), 1);
    input_channel_dropdown.addSeparator();
    input_channel_dropdown.addItemList(input_channels.at(ChannelLayout::Stereo), input_channel_dropdown.getNumItems()+1);

    input_channel_dropdown.setSelectedId(1);
}

void TrackHeaderComponent::update_output_channel_dropdown() {
    output_channel_dropdown.clear(juce::dontSendNotification);
    if (output_dst_dropdown.getSelectedId() != static_cast<int>(OutputDestination::External)) {
        output_channel_dropdown.setEnabled(false);
        return;
    }

    const auto& output_channels = audio_engine.get_channel_manager().get_output_channel_strings();

    output_channel_dropdown.setEnabled(true);

    output_channel_dropdown.addItemList(output_channels.at(ChannelLayout::Mono), 1);
    output_channel_dropdown.addSeparator();
    output_channel_dropdown.addItemList(output_channels.at(ChannelLayout::Stereo), output_channel_dropdown.getNumItems()+1);

    output_channel_dropdown.setSelectedId(1);
}

void TrackHeaderComponent::update_input_channel() {
    auto& controls = audio_engine.get_tracks()[track_id]->get_mixer_controls();

    controls.input_src = static_cast<InputSource>(input_src_dropdown.getSelectedId());
    if (controls.input_src != InputSource::External) {
        controls.input_src_channel = 0;
        controls.input_layout = ChannelLayout::Mono;
        controls.input_buffer_idx = -1;
        return;
    }

    const auto pair = audio_engine.get_channel_manager().get_input_channel_from_string(input_channel_dropdown.getText());
    if (pair.first < 0) {
        return;
    }

    controls.input_src_channel = pair.first;
    controls.input_layout = pair.second;
    controls.input_buffer_idx = audio_engine.get_channel_manager().input_channel_to_buffer_idx(controls.input_src_channel);

    DBG("update input channel");
    DBG(controls.input_src_channel.load());
    DBG(static_cast<int>(controls.input_layout.load()));
    DBG(controls.input_buffer_idx.load());
}

void TrackHeaderComponent::update_output_channel() {
    auto& controls = audio_engine.get_tracks()[track_id]->get_mixer_controls();

    controls.output_dest = static_cast<OutputDestination>(output_dst_dropdown.getSelectedId());
    if (controls.output_dest != OutputDestination::External) {
        controls.output_dest_channel = 0;
        controls.output_layout = ChannelLayout::Mono;
        controls.output_buffer_idx = -1;
        return;
    }

    const auto pair = audio_engine.get_channel_manager().get_output_channel_from_string(output_channel_dropdown.getText());

    controls.output_dest_channel = pair.first;
    controls.output_layout = pair.second;
    controls.output_buffer_idx = audio_engine.get_channel_manager().output_channel_to_buffer_idx(controls.output_dest_channel);
}

void TrackHeaderComponent::set_on_delete(std::function<void()> fn) {
    on_delete = fn;
}
