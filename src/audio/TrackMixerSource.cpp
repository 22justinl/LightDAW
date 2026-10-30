#include "audio/TrackMixerSource.h"
#include "juce_core/juce_core.h"

TrackMixerSource::TrackMixerSource(Transport& transport_): transport(transport_) { }

void TrackMixerSource::setNextReadPosition(SamplePosition newPosition)  {
    read_position = std::min(newPosition, total_length);
}

SamplePosition TrackMixerSource::getNextReadPosition() const  {
    return transport.get_position();
}

SamplePosition TrackMixerSource::getTotalLength() const  {
    return transport.get_end_pos();
}

bool TrackMixerSource::isLooping() const  {
    return false;
}

void TrackMixerSource::setLooping(bool shouldLoop)  {
    juce::ignoreUnused(shouldLoop);
    throw std::runtime_error("Not implemented");
}

void TrackMixerSource::prepareToPlay(int samplePerBlockExpected, double sampleRate) {
    mixer_source.prepareToPlay(samplePerBlockExpected, sampleRate);
}

void TrackMixerSource::releaseResources()  {
    mixer_source.releaseResources();
}

void TrackMixerSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)  {
    mixer_source.getNextAudioBlock(bufferToFill);
}

void TrackMixerSource::addInputSource(AudioSource *newInput) {
    mixer_source.addInputSource(newInput, false);
}

void TrackMixerSource::removeInputSource(AudioSource *input) {
    mixer_source.removeInputSource(input);
}
