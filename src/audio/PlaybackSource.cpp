#include "audio/PlaybackSource.h"
#include "juce_core/juce_core.h"

PlaybackSource::PlaybackSource(AudioEngine& audio_engine_): audio_engine(audio_engine_) {
    juce::ignoreUnused(audio_engine);
}

void PlaybackSource::setNextReadPosition(juce::int64 newPosition)  {
    read_position = std::min(newPosition, total_length);
}

juce::int64 PlaybackSource::getNextReadPosition() const  {
    return read_position;
}

juce::int64 PlaybackSource::getTotalLength() const  {
    return total_length;
}

bool PlaybackSource::isLooping() const  {
    return false;
}

void PlaybackSource::setLooping(bool shouldLoop)  {
    juce::ignoreUnused(shouldLoop);
    throw std::runtime_error("Not implemented");
}

void PlaybackSource::prepareToPlay(int samplePerBlockExpected, double sampleRate)  {
    juce::ignoreUnused(samplePerBlockExpected, sampleRate);
}

void PlaybackSource::releaseResources()  {

}

void PlaybackSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)  {
    juce::ignoreUnused(bufferToFill);
}
