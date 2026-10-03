#pragma once

#include "audio/Transport.h"

#include <juce_audio_basics/juce_audio_basics.h>

struct Clip;

class TrackAudioSource: public juce::PositionableAudioSource {
public:
    TrackAudioSource(Transport& transport_, std::vector<Clip>& clips_);
    void setNextReadPosition(SamplePosition newPosition) override;
    SamplePosition getNextReadPosition() const override;
    SamplePosition getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

    void prepareToPlay(int samplePerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
private:
    Transport& transport;
    std::vector<Clip>& clips;
    size_t current_clip = 0;
};
