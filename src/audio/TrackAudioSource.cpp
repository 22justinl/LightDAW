#include "audio/TrackAudioSource.h"

#include "audio/Track.h"

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include <algorithm>

constexpr float MONO_TO_STEREO_PAN_FACTOR = juce::MathConstants<float>::sqrt2 / 2;

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
    smoothed_pan_l.reset(sampleRate, 2e-2);
    smoothed_pan_r.reset(sampleRate, 2e-2);
}

void TrackAudioSource::releaseResources() { }

void TrackAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    int n = bufferToFill.numSamples;
    float* buffers[2] = {bufferToFill.buffer->getWritePointer(0, 0), bufferToFill.buffer->getWritePointer(1, 0)};

    // update smooth parameters
    float pan = mixer_controls.get_pan();
    smoothed_pan_l.setTargetValue(std::max(0.f, 1-pan));
    smoothed_pan_r.setTargetValue(std::max(0.f, 1+pan));

    // monitoring
    if (mixer_controls.monitoring.load() && input_view.input_channel_data && mixer_controls.input_buffer_idx >= 0) {
        if (mixer_controls.input_layout == ChannelLayout::Mono) {
            juce::FloatVectorOperations::multiply(buffers[0], input_view.input_channel_data[mixer_controls.input_buffer_idx], MONO_TO_STEREO_PAN_FACTOR, n);
            juce::FloatVectorOperations::multiply(buffers[1], input_view.input_channel_data[mixer_controls.input_buffer_idx], MONO_TO_STEREO_PAN_FACTOR, n);
        } else {
            juce::FloatVectorOperations::copy(buffers[0], input_view.input_channel_data[mixer_controls.input_buffer_idx], n);
            juce::FloatVectorOperations::copy(buffers[1], input_view.input_channel_data[mixer_controls.input_buffer_idx+1], n);
        }
    }

    if (!transport.is_playing() || current_clip >= clips.size()) {
        return;
    }

    // playback
    while (transport.is_playing() && current_clip < clips.size() && n > 0) {
        if (transport.get_position() < clips[current_clip].pos) {
            break;
        }
        // TODO: handle Mono and Stereo clips, currently assuming clip is stereo
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

void TrackAudioSource::process_audio_block(const juce::AudioSourceChannelInfo& bufferToFill) {
    int n = bufferToFill.numSamples;
    float* buffers[2] = {bufferToFill.buffer->getWritePointer(0, 0), bufferToFill.buffer->getWritePointer(1, 0)};

    for (int i = 0; i < n; ++i) {
        buffers[0][i] *= smoothed_pan_l.getNextValue();
        buffers[1][i] *= smoothed_pan_r.getNextValue();
    }
}

const TrackMixerControls& TrackAudioSource::get_mixer_controls() const {
    return mixer_controls;
}
