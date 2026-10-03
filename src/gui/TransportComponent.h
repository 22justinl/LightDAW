#pragma once

#include "audio/AudioEngine.h"
#include "juce_events/juce_events.h"
#include "juce_gui_basics/juce_gui_basics.h"

class TransportComponent: public juce::Component, private juce::Timer {
public:
    TransportComponent(AudioEngine& audio_engine_);

    void resized() override;
private:
    void timerCallback() override;

    void update_play_button();

    AudioEngine& audio_engine;

    juce::TextButton play_button {"Play"};
    juce::TextButton stop_button {"Stop"};
    juce::Label position_label;
    juce::TextButton wake_button {"Wake"};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};
