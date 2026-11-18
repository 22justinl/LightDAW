#include "audio/TrackMixerControls.h"

#include <algorithm>

float TrackMixerControls::get_gain() const { return gain_db.load(); }
void TrackMixerControls::set_gain(float new_gain_db) { gain_db.store(new_gain_db); }

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
