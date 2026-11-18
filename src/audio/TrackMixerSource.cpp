#include "audio/TrackMixerSource.h"
#include "audio/TrackMixerControls.h"

#include <juce_core/juce_core.h>

TrackMixerSource::TrackMixerSource(Transport& transport_, ChannelManager& channel_manager_, std::unordered_map<TrackId, std::unique_ptr<Track>>& tracks_): transport(transport_), channel_manager(channel_manager_), tracks(tracks_) {}

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
    temp_buffer.setSize(2, samplePerBlockExpected);
    temp_buffer_info = juce::AudioSourceChannelInfo(temp_buffer);

    for (auto& p : tracks) {
        Track& track = *p.second;
        track.get_audio_source().prepareToPlay(samplePerBlockExpected, sampleRate);
    }
}

void TrackMixerSource::releaseResources()  {
    for (auto& p : tracks) {
        Track& track = *p.second;
        track.get_audio_source().releaseResources();
    }
}

void TrackMixerSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)  {
    bufferToFill.clearActiveBufferRegion();
    juce::AudioBuffer<float>* buffer = bufferToFill.buffer;
    int n = bufferToFill.numSamples;

    for (auto& p : tracks) {
        Track& track = *p.second;
        temp_buffer_info.clearActiveBufferRegion();
        route_track_audio_to_output(track.get_audio_source(), buffer, n);
    }
}

void TrackMixerSource::increment_solo_count() {
    ++solo_count;
}

void TrackMixerSource::decrement_solo_count() {
    --solo_count;
}

void TrackMixerSource::route_track_audio_to_output(TrackAudioSource& source, juce::AudioBuffer<float>* buffer, int n) {
    source.getNextAudioBlock(temp_buffer_info);

    const TrackMixerControls& mixer_controls = source.get_mixer_controls();

    // TODO: handle recording here

    if (mixer_controls.is_muted() || (solo_count.load() > 0 && !mixer_controls.is_soloed())) {
        return;
    }

    source.process_audio_block(temp_buffer_info);

    OutputDestination output_dest = mixer_controls.get_output_dest();
    ChannelLayout layout = mixer_controls.get_output_layout();
    ChannelId channel = mixer_controls.get_output_channel();

    if (output_dest == OutputDestination::Master) {
        channel = channel_manager.get_default_output_channel(ChannelLayout::Stereo); // TODO: change to make master output editable
        layout = ChannelLayout::Stereo;
    }

    if (!channel_manager.is_output_channel_active(channel, layout)) {
        return;
    }

    int idx = static_cast<int>(channel_manager.output_channel_to_buffer_idx(channel));
    if (idx < 0) { return; }

    if (layout == ChannelLayout::Mono) {
        buffer->addFrom(idx, 0, temp_buffer, 0, 0, n, 0.5);
        buffer->addFrom(idx, 0, temp_buffer, 1, 0, n, 0.5);
    } else {
        buffer->addFrom(idx, 0, temp_buffer, 0, 0, n);
        idx = static_cast<int>(channel_manager.output_channel_to_buffer_idx(channel+1));
        if (idx < 0) { return; }
        buffer->addFrom(idx, 0, temp_buffer, 1, 0, n);
    }
}
