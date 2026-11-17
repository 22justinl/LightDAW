#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class LevelMeterComponent: public juce::Component {
public:
    LevelMeterComponent();
    void paint (juce::Graphics& g) override;
    void resized() override;
private:
};
