#pragma once

#include "audio/Track.h"
#include <juce_gui_basics/juce_gui_basics.h>

class ClipComponent: public juce::Component {
public:
    ClipComponent(Track& track, ClipId clip_id_);
    void paint(juce::Graphics &g) override;
    void resized() override;
private:
    Track& track;
    juce::String name;
    const ClipId clip_id;
};
