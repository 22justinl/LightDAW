#include "audio/TrackAudioSource.h"

#include "audio/Track.h"
#include "juce_audio_basics/juce_audio_basics.h"

#include <juce_core/juce_core.h>

TrackAudioSource::TrackAudioSource(Transport& transport_, std::vector<Clip>& clips_, AudioInputView& input_view_, TrackMixerControls& mixer_controls_)
    :   transport(transport_),
        clips(clips_),
        input_view(input_view_),
        mixer_controls(mixer_controls_) {}

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

void TrackAudioSource::releaseResources() { }

void TrackAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    bufferToFill.clearActiveBufferRegion();

    if (mixer_controls.muted.load()) {
        return;
    }
    int n = bufferToFill.numSamples;
    float* buffers[2] = {bufferToFill.buffer->getWritePointer(0, 0), bufferToFill.buffer->getWritePointer(1, 0)};


    // TODO: handle mono/stereo input for both monitor audio and clip audio (use separate function for all processing?)
    if (mixer_controls.monitoring.load() && input_view.input_channel_data && mixer_controls.input_buffer_idx >= 0) {
        juce::FloatVectorOperations::copy(buffers[0], input_view.input_channel_data[mixer_controls.input_buffer_idx], n);
        if (mixer_controls.input_layout == ChannelLayout::Stereo) {
            juce::FloatVectorOperations::copy(buffers[1], input_view.input_channel_data[mixer_controls.input_buffer_idx+1], n);
        } else {
            juce::FloatVectorOperations::copy(buffers[1], input_view.input_channel_data[mixer_controls.input_buffer_idx], n);
        }
    }

    if (!transport.is_playing() || current_clip >= clips.size()) {
        return;
    }
    while (transport.is_playing() && current_clip < clips.size() && n > 0) {
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

OutputDestination TrackAudioSource::get_output_dest() const {
    return mixer_controls.output_dest.load();
}
ChannelId TrackAudioSource::get_output_channel() const {
    return mixer_controls.output_buffer_idx.load();
}
ChannelLayout TrackAudioSource::get_output_layout() const {
    return mixer_controls.output_layout.load();
}
