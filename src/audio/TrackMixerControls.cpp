#include "audio/TrackMixerControls.h"

#include <algorithm>

float TrackMixerControls::get_volume() const { return volume.load(); }
float TrackMixerControls::get_gain() const { return gain.load(); }
void TrackMixerControls::set_volume(float new_volume) { volume.store(new_volume); gain.store(juce::Decibels::decibelsToGain(new_volume, -60.f)); }

float TrackMixerControls::get_pan() const { return pan.load(); }
void TrackMixerControls::set_pan(float new_pan) { pan.store(std::clamp(new_pan, -1.f, 1.f)); }

bool TrackMixerControls::is_muted() const { return muted.load(); }
void TrackMixerControls::set_muted(bool new_muted) { muted.store(new_muted); }

bool TrackMixerControls::is_soloed() const { return soloed.load(); }
void TrackMixerControls::set_soloed(bool new_soloed) { soloed.store(new_soloed); }

bool TrackMixerControls::is_recording() const { return recording.load(); }
void TrackMixerControls::set_recording(bool new_recording) { recording.store(new_recording); }

bool TrackMixerControls::is_monitoring() const { return monitoring.load(); }
void TrackMixerControls::set_monitoring(bool new_monitoring) { monitoring.store(new_monitoring); }

OutputDestination TrackMixerControls::get_output_dest() const {
    return output_dest.load();
}
ChannelId TrackMixerControls::get_output_channel() const {
    return output_buffer_idx.load();
}
ChannelLayout TrackMixerControls::get_output_layout() const {
    return output_layout.load();
}
