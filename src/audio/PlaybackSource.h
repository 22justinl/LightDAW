#pragma once

#include "Types.h"
#include "audio/Transport.h"

#include <juce_audio_basics/juce_audio_basics.h>

class PlaybackSource: public juce::PositionableAudioSource {
public:
    PlaybackSource(Transport& transport_);
    void setNextReadPosition(SamplePosition newPosition) override;
    SamplePosition getNextReadPosition() const override;
    SamplePosition getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

    void prepareToPlay(int samplePerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    void addInputSource(AudioSource *newInput);
    void removeInputSource(AudioSource *input);
private:
    Transport& transport;

    SamplePosition read_position;
    SamplePosition total_length;

    juce::MixerAudioSource mixer_source;
};
