#include "audio/TrackAudioSource.h"

#include "audio/Track.h"

#include <juce_core/juce_core.h>

TrackAudioSource::TrackAudioSource(Transport& transport_, std::vector<Clip>& clips_): transport(transport_), clips(clips_) {}

void TrackAudioSource::setNextReadPosition(SamplePosition newPosition) {
    juce::ignoreUnused(newPosition);
}

SamplePosition TrackAudioSource::getNextReadPosition() const {
    return transport.get_position();
}

SamplePosition TrackAudioSource::getTotalLength() const {
    return transport.get_end_pos();
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
    current_clip = 0;
}

void TrackAudioSource::releaseResources() {

}

void TrackAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    bufferToFill.clearActiveBufferRegion();
    if (current_clip >= clips.size()) {
        return;
    }
    // int num_channels = bufferToFill.buffer->getNumChannels();
    int n = bufferToFill.numSamples;
    float* buffers[2] = {bufferToFill.buffer->getWritePointer(0, 0), bufferToFill.buffer->getWritePointer(1, 0)};
    while (current_clip < clips.size() && n > 0 && transport.is_playing()) {
        if (transport.get_position() < clips[current_clip].pos) {
            break;
        }
        if (!clips[current_clip].stream || clips[current_clip].stream->is_done()) {
            ++current_clip;
            continue;
        }
        int read = clips[current_clip].stream->read(buffers, n);
        clips[current_clip].stream->check_and_queue_refill();
        buffers[0] += read;
        buffers[1] += read;
        n -= read;
    }
}
