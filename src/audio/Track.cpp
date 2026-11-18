#include "audio/Track.h"
#include "juce_core/juce_core.h"

// PUBLIC FUNCTIONS

Track::Track(Transport& transport_, TrackId id_, AudioInputView& input_view_)
    :   transport(transport_),
        id(id_),
        name("Track " + std::to_string(id_)),
        audio_source(transport_, clips, input_view_, mixer_controls) { }
Track::Track(Transport& transport_, TrackId id_, AudioInputView& input_view_, const juce::String& name_)
    :   transport(transport_),
        id(id_),
        name(name_),
        audio_source(transport_, clips, input_view_, mixer_controls) { }

Track::~Track() {
    for (auto& c : clips) {
        c.stream = nullptr;
    }
}

const juce::String& Track::get_name() const { return name; }
void Track::set_name(const juce::String& new_name) { name = new_name; }

float Track::get_gain() const { return mixer_controls.get_gain(); }
void Track::set_gain(float new_gain_db) { mixer_controls.set_gain(new_gain_db); }

float Track::get_pan() const { return mixer_controls.get_pan(); }
void Track::set_pan(float new_gain_db) { mixer_controls.set_pan(new_gain_db); }

bool Track::is_muted() const { return mixer_controls.is_muted(); }
void Track::set_muted(bool new_muted) { mixer_controls.set_muted(new_muted); }

bool Track::is_soloed() const { return mixer_controls.is_soloed(); }
void Track::set_soloed(bool new_soloed) { mixer_controls.set_soloed(new_soloed); }

bool Track::is_recording() const { return mixer_controls.is_recording(); }
void Track::set_recording(bool new_recording) { mixer_controls.set_recording(new_recording); }

bool Track::is_monitoring() const { return mixer_controls.is_monitoring(); }
void Track::set_monitoring(bool new_monitoring) { mixer_controls.set_monitoring(new_monitoring); }

TrackAudioSource& Track::get_audio_source() {
    return audio_source;
}

SamplePosition Track::get_max_sample_position() const {
    if (clips.empty()) { return 0; }
    return clips.back().end_pos;
}

const Clip* Track::get_clip_by_id(ClipId clip_id) const {
    for (size_t i = 0; i < clips.size(); ++i) {
        if (clips[i].id == clip_id) {
            return &clips[i];
        }
    }
    return nullptr;
}

void Track::import_file(const juce::File& file, SamplePosition pos, SamplePosition start, SamplePosition end) {
    size_t i;
    for (i = 0; i < clips.size(); ++i) {
        if (pos <= clips[i].start) {
            break;
        }
    }
    clips.insert(clips.begin()+static_cast<int>(i), {next_clip_id, file, pos, start, end});
    clip_change_queue.emplace(next_clip_id++, ClipChangeType::Add);
    sendChangeMessage();
}

TrackMixerControls& Track::get_mixer_controls() {
    return mixer_controls;
}
