#include "gui/TrackHeaderComponent.h"

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>

// PUBLIC FUNCTIONS

TrackHeaderComponent::TrackHeaderComponent(AudioEngine& audio_engine_, TrackId track_id_)
    :   audio_engine(audio_engine_),
        track_id(track_id_) {
    audio_engine.get_channel_manager().addChangeListener(this);

    track_number_label.setLookAndFeel(&look_and_feel);
    track_number_label.setText(juce::String(track_id), juce::dontSendNotification);
    addAndMakeVisible(track_number_label);

    track_name_label.setLookAndFeel(&look_and_feel);
    track_name_label.setText(audio_engine.get_tracks()[track_id]->get_name(), juce::dontSendNotification);
    track_name_label.setJustificationType(juce::Justification::topLeft);
    track_name_label.setEditable(false, true, false);
    track_name_label.onEditorHide = [&]() { audio_engine.track_set_name(track_id, track_name_label.getText(true)); };
    addAndMakeVisible(track_name_label);


    input_src_dropdown.setLookAndFeel(&look_and_feel);
    input_src_dropdown.addItem("Ext. In", static_cast<int>(InputSource::External));
    input_src_dropdown.setSelectedId(static_cast<int>(InputSource::External), juce::dontSendNotification);
    input_src_dropdown.onChange = [&]() { update_input_channel_dropdown(); };
    addAndMakeVisible(input_src_dropdown);

    input_channel_dropdown.setLookAndFeel(&look_and_feel);
    input_channel_dropdown.onChange = [&]() { update_input_channel(); };
    addAndMakeVisible(input_channel_dropdown);

    output_dst_dropdown.setLookAndFeel(&look_and_feel);
    output_dst_dropdown.addItem("Master", static_cast<int>(OutputDestination::Master));
    output_dst_dropdown.addItem("Ext. Out", static_cast<int>(OutputDestination::External));
    output_dst_dropdown.setSelectedId(static_cast<int>(OutputDestination::Master), juce::dontSendNotification);
    output_dst_dropdown.onChange = [&]() { update_output_channel_dropdown(); };
    addAndMakeVisible(output_dst_dropdown);

    output_channel_dropdown.setLookAndFeel(&look_and_feel);
    output_channel_dropdown.onChange = [&]() { update_output_channel(); };
    addAndMakeVisible(output_channel_dropdown);


    mute_button.setLookAndFeel(&look_and_feel);
    mute_button.setToggleable(true);
    mute_button.setToggleState(true, juce::dontSendNotification);
    mute_button.setClickingTogglesState(true);
    mute_button.onClick = [&](){
        audio_engine.track_set_muted(track_id, !audio_engine.track_is_muted(track_id));
    };
    addAndMakeVisible(mute_button);

    solo_button.setLookAndFeel(&look_and_feel);
    solo_button.setToggleable(true);
    solo_button.setToggleState(true, juce::dontSendNotification);
    solo_button.setClickingTogglesState(true);
    solo_button.onClick = [&](){
        audio_engine.track_set_soloed(track_id, !audio_engine.track_is_soloed(track_id));
    };

    addAndMakeVisible(solo_button);
    recording_button.setLookAndFeel(&look_and_feel);
    recording_button.setToggleable(true);
    recording_button.setToggleState(true, juce::dontSendNotification);
    recording_button.setClickingTogglesState(true);
    recording_button.onClick = [&](){
        audio_engine.track_set_recording(track_id, !audio_engine.track_is_recording(track_id));
    };

    addAndMakeVisible(recording_button);
    monitoring_button.setLookAndFeel(&look_and_feel);
    monitoring_button.setToggleable(true);
    monitoring_button.setToggleState(true, juce::dontSendNotification);
    monitoring_button.setClickingTogglesState(true);
    monitoring_button.onClick = [&](){
        audio_engine.track_set_monitoring(track_id, !audio_engine.track_is_monitoring(track_id));
    };
    addAndMakeVisible(monitoring_button);


    volume_slider.setLookAndFeel(&look_and_feel);
    volume_slider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    volume_slider.setScrollWheelEnabled(false);
    volume_slider.setNumDecimalPlacesToDisplay(1);
    volume_slider.setTextBoxIsEditable(false);
    volume_slider.setDoubleClickReturnValue(true, 0);
    volume_slider.setRange(-60, 6, 0.1);
    volume_slider.setSkewFactor(10, false);
    volume_slider.setSkewFactorFromMidPoint(-24);
    volume_slider.textFromValueFunction = [](double value) { return juce::Decibels::toString(value, 1, -60.0); };
    volume_slider.onValueChange = [&]() { audio_engine.track_set_volume(track_id, static_cast<float>(volume_slider.getValue())); };
    volume_slider.updateText();
    addAndMakeVisible(volume_slider);

    pan_slider.setLookAndFeel(&look_and_feel);
    pan_slider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    pan_slider.setScrollWheelEnabled(false);
    pan_slider.setNumDecimalPlacesToDisplay(0);
    pan_slider.setTextBoxIsEditable(false);
    pan_slider.setRange(-1, 1, 0.02);
    pan_slider.textFromValueFunction = [](double value) { int int_val = static_cast<int>(50*value); return int_val == 0 ? "C" : (int_val > 0 ? std::to_string(int_val)+"R" : std::to_string(int_val)+"L"); };
    pan_slider.onValueChange = [&]() { audio_engine.track_set_pan(track_id, static_cast<float>(pan_slider.getValue())); };
    pan_slider.updateText();
    addAndMakeVisible(pan_slider);


    update_input_channel_dropdown();
    update_output_channel_dropdown();
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
    TrackHeaderLayout layout = calculate_layout();

    g.fillAll(juce::Colours::grey);

    g.setColour(juce::Colours::black);
    g.fillRect(layout.number_name_border);
    g.fillRect(layout.name_channel_border);
    g.fillRect(layout.channel_mixer_border);

    g.setColour(juce::Colours::darkgrey);
    g.fillRect(layout.number);
}

void TrackHeaderComponent::resized() {
    TrackHeaderLayout layout = calculate_layout();

    track_number_label.setBounds(layout.number);
    track_name_label.setBounds(layout.name);

    input_src_dropdown.setBounds(layout.input_src);
    input_channel_dropdown.setBounds(layout.input_channel);
    output_dst_dropdown.setBounds(layout.output_dst);
    output_channel_dropdown.setBounds(layout.output_channel);

    mute_button.setBounds(layout.mute);
    solo_button.setBounds(layout.solo);
    recording_button.setBounds(layout.recording);
    monitoring_button.setBounds(layout.monitoring);

    volume_slider.setBounds(layout.volume);
    pan_slider.setBounds(layout.pan);
}

void TrackHeaderComponent::changeListenerCallback(juce::ChangeBroadcaster*) {
    update_input_channel_dropdown();
    update_output_channel_dropdown();
}

void TrackHeaderComponent::set_on_delete(std::function<void()> fn) {
    on_delete = fn;
}

TrackHeaderLayout TrackHeaderComponent::calculate_layout() const {
    TrackHeaderLayout layout;

    auto area = getLocalBounds();

    layout.number = area.removeFromLeft(36);

    layout.number_name_border = area.removeFromLeft(border_width);

    area.removeFromRight(spacing);
    layout.volume_meter = area.removeFromRight(5);
    layout.volume_meter.removeFromTop(spacing);
    layout.volume_meter.removeFromBottom(spacing);
    area.removeFromRight(spacing);
    auto mixer_control = area.removeFromRight(4*item_width + 3*spacing);
    mixer_control.removeFromTop(spacing);
    mixer_control.removeFromBottom(spacing);
    area.removeFromRight(spacing);

    layout.channel_mixer_border = area.removeFromRight(border_width);

    area.removeFromRight(spacing);
    auto channel_selection = area.removeFromRight(4*item_width + 3*spacing);
    channel_selection.removeFromTop(spacing);
    channel_selection.removeFromBottom(spacing);
    area.removeFromRight(spacing);

    layout.name_channel_border = area.removeFromRight(border_width);

    layout.name = area;
    layout.name.removeFromTop(spacing);
    layout.name.removeFromBottom(spacing);

    layout.input_src = channel_selection.removeFromTop(item_height);
    channel_selection.removeFromTop(spacing);
    layout.input_channel = channel_selection.removeFromTop(item_height);
    channel_selection.removeFromTop(spacing);
    layout.output_dst = channel_selection.removeFromTop(item_height);
    channel_selection.removeFromTop(spacing);
    layout.output_channel = channel_selection.removeFromTop(item_height);

    auto msri = mixer_control.removeFromTop(item_height);
    mixer_control.removeFromTop(spacing);
    auto volume_pan = mixer_control.removeFromTop(item_height);

    layout.mute = msri.removeFromLeft(item_width);
    msri.removeFromLeft(spacing);
    layout.solo = msri.removeFromLeft(item_width);
    msri.removeFromLeft(spacing);
    layout.recording = msri.removeFromLeft(item_width);
    msri.removeFromLeft(spacing);
    layout.monitoring = msri.removeFromLeft(item_width);

    layout.volume = volume_pan.removeFromLeft(2*item_width+spacing);
    volume_pan.removeFromLeft(spacing);
    layout.pan = volume_pan.removeFromLeft(2*item_width+spacing);

    return layout;
}

void TrackHeaderComponent::update_input_channel_dropdown() {
    input_channel_dropdown.clear(juce::dontSendNotification);
    if (input_src_dropdown.getSelectedId() != static_cast<int>(InputSource::External)) {
        input_channel_dropdown.setEnabled(false);
        return;
    }

    const auto& input_channels = audio_engine.get_channel_manager().get_input_channel_strings();
    if (input_channels.empty()) { return; }

    input_channel_dropdown.setEnabled(true);

    if (!input_channels.at(ChannelLayout::Mono).isEmpty()) {
        input_channel_dropdown.addItemList(input_channels.at(ChannelLayout::Mono), 1);
    }
    input_channel_dropdown.addSeparator();
    if (!input_channels.at(ChannelLayout::Mono).isEmpty()) {
        input_channel_dropdown.addItemList(input_channels.at(ChannelLayout::Stereo), input_channel_dropdown.getNumItems()+1);
    }

    if (input_channel_dropdown.getNumItems() == 0) {
        input_channel_dropdown.setEnabled(false);
    } else {
        input_channel_dropdown.setSelectedId(1);
    }
}

void TrackHeaderComponent::update_output_channel_dropdown() {
    output_channel_dropdown.clear(juce::dontSendNotification);
    if (output_dst_dropdown.getSelectedId() != static_cast<int>(OutputDestination::External)) {
        output_channel_dropdown.setEnabled(false);
        return;
    }

    const auto& output_channels = audio_engine.get_channel_manager().get_output_channel_strings();
    if (output_channels.empty()) { return; }

    output_channel_dropdown.setEnabled(true);

    if (!output_channels.at(ChannelLayout::Mono).isEmpty()) {
        output_channel_dropdown.addItemList(output_channels.at(ChannelLayout::Mono), 1);
    }
    output_channel_dropdown.addSeparator();
    if (!output_channels.at(ChannelLayout::Stereo).isEmpty()) {
        output_channel_dropdown.addItemList(output_channels.at(ChannelLayout::Stereo), output_channel_dropdown.getNumItems()+1);
    }

    if (output_channel_dropdown.getNumItems() == 0) {
        output_channel_dropdown.setEnabled(false);
    } else {
        output_channel_dropdown.setSelectedId(1);
    }
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
    if (pair.first < 0) {
        return;
    }

    controls.output_dest_channel = pair.first;
    controls.output_layout = pair.second;
    controls.output_buffer_idx = audio_engine.get_channel_manager().output_channel_to_buffer_idx(controls.output_dest_channel);
}
