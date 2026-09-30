#include "audio/TrackAudioSource.h"

#include "audio/Track.h"

#include <juce_core/juce_core.h>

TrackAudioSource::TrackAudioSource(Transport& transport_, std::vector<Clip>& clips_): transport(transport_), clips(clips_) {}

void TrackAudioSource::setNextReadPosition(juce::int64 newPosition) {
    juce::ignoreUnused(newPosition);
}

juce::int64 TrackAudioSource::getNextReadPosition() const {
    return transport.get_position();
}

juce::int64 TrackAudioSource::getTotalLength() const {
    // return transport.getTotalLength();
    return INT_MAX;
}

bool TrackAudioSource::isLooping() const {
    return false;
}

void TrackAudioSource::setLooping(bool shouldLoop) {
    DBG("Looping not supported");
    juce::ignoreUnused(shouldLoop);
}


void TrackAudioSource::prepareToPlay(int samplePerBlockExpected, double sampleRate) {
    juce::ignoreUnused(samplePerBlockExpected, sampleRate);
}

void TrackAudioSource::releaseResources() {

}

void TrackAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    juce::ignoreUnused(bufferToFill);
}
