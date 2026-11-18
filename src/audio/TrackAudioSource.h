#pragma once

#include "audio/TrackMixerControls.h"
#include "audio/Transport.h"
#include "audio/AudioInputView.h"

#include <juce_audio_basics/juce_audio_basics.h>

struct Clip;

class TrackAudioSource: public juce::PositionableAudioSource {
public:
    TrackAudioSource(Transport& transport_, std::vector<Clip>& clips_, AudioInputView& input_view_, TrackMixerControls& mixer_controls_);
    void setNextReadPosition(SamplePosition newPosition) override;
    SamplePosition getNextReadPosition() const override;
    SamplePosition getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

    void prepareToPlay(int samplePerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    const TrackMixerControls& get_mixer_controls() const;

    void process_audio_block(const juce::AudioSourceChannelInfo& buffer);
private:

    Transport& transport;
    std::vector<Clip>& clips;
    AudioInputView& input_view;
    TrackMixerControls& mixer_controls;

    juce::SmoothedValue<float> smoothed_gain;

    juce::SmoothedValue<float> smoothed_pan_l;
    juce::SmoothedValue<float> smoothed_pan_r;

    size_t current_clip = 0;
};
