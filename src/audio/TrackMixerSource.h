#pragma once

#include "Types.h"
#include "audio/ChannelManager.h"
#include "audio/Track.h"
#include "audio/TrackAudioSource.h"
#include "audio/Transport.h"

#include <juce_audio_basics/juce_audio_basics.h>

class TrackMixerSource: public juce::PositionableAudioSource {
public:
    TrackMixerSource(Transport& transport_, ChannelManager& channel_manager_, std::unordered_map<TrackId, std::unique_ptr<Track>>& tracks_);
    void setNextReadPosition(SamplePosition newPosition) override;
    SamplePosition getNextReadPosition() const override;
    SamplePosition getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

    void prepareToPlay(int samplePerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
private:
    void route_track_audio_to_output(TrackAudioSource& source, juce::AudioBuffer<float>* buffer, int n);

    Transport& transport;
    ChannelManager& channel_manager;
    std::unordered_map<TrackId, std::unique_ptr<Track>>& tracks;

    SamplePosition read_position;
    SamplePosition total_length;

    juce::AudioBuffer<float> temp_buffer;
    juce::AudioSourceChannelInfo temp_buffer_info;
};
