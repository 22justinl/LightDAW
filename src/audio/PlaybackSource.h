#pragma once

#include "audio/AudioEngine.h"

#include <juce_audio_basics/juce_audio_basics.h>

class PlaybackSource: public juce::PositionableAudioSource {
public:
    PlaybackSource(AudioEngine& audio_engine_);
    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

    void prepareToPlay(int samplePerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
private:
    AudioEngine& audio_engine;
    juce::int64 read_position;
    juce::int64 total_length;

    juce::MixerAudioSource mixer_source;
};
