#include "audio/PlaybackSource.h"
#include "juce_core/juce_core.h"

PlaybackSource::PlaybackSource(Transport& transport_): transport(transport_) { }

void PlaybackSource::setNextReadPosition(SamplePosition newPosition)  {
    read_position = std::min(newPosition, total_length);
}

SamplePosition PlaybackSource::getNextReadPosition() const  {
    return transport.get_position();
}

SamplePosition PlaybackSource::getTotalLength() const  {
    return transport.get_end_pos();
}

bool PlaybackSource::isLooping() const  {
    return false;
}

void PlaybackSource::setLooping(bool shouldLoop)  {
    juce::ignoreUnused(shouldLoop);
    throw std::runtime_error("Not implemented");
}

void PlaybackSource::prepareToPlay(int samplePerBlockExpected, double sampleRate) {
    mixer_source.prepareToPlay(samplePerBlockExpected, sampleRate);
}

void PlaybackSource::releaseResources()  {
    mixer_source.releaseResources();
}

void PlaybackSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)  {
    mixer_source.getNextAudioBlock(bufferToFill);
}

void PlaybackSource::addInputSource(AudioSource *newInput) {
    mixer_source.addInputSource(newInput, false);
}

void PlaybackSource::removeInputSource(AudioSource *input) {
    mixer_source.removeInputSource(input);
}
