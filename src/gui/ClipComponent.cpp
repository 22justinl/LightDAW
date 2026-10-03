#include "gui/ClipComponent.h"

ClipComponent::ClipComponent(Track& track_, ClipId clip_id_): track(track_), clip_id(clip_id_) {
    const Clip* clip = track.get_clip_by_id(clip_id);
    if (!clip) { throw std::runtime_error("Clip with id " + std::to_string(clip_id) + " not found in track " + track.get_name().toStdString()); }
    name = clip->file.getFileName();
}

void ClipComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);

    g.setColour(juce::Colours::white);
    g.drawText(
        name,
        getLocalBounds().reduced(5),
        juce::Justification::centredLeft,
        true);
}

void ClipComponent::resized() {

}
