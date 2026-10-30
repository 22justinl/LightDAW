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

float Track::get_gain() const { return mixer_controls.gain_db.load(); }
void Track::set_gain(float new_gain_db) { mixer_controls.gain_db.store(new_gain_db); }

bool Track::is_muted() const { return mixer_controls.muted.load(); }
void Track::set_muted(bool new_muted) { mixer_controls.muted.store(new_muted); }

bool Track::is_monitoring() const { return mixer_controls.monitoring.load(); }
void Track::set_monitoring(bool new_monitoring) { mixer_controls.monitoring.store(new_monitoring); }

const juce::String& Track::get_selected_device_name() const { return mixer_controls.selected_device_name; }
void Track::set_selected_device_name(const juce::String& new_device_name) { mixer_controls.selected_device_name = new_device_name; }

ChannelId Track::get_selected_channel() const { return mixer_controls.selected_channel; }
void Track::set_selected_channel(ChannelId new_channel) { mixer_controls.selected_channel = new_channel; }

ChannelId Track::get_channel() const { return mixer_controls.channel; }
void Track::set_channel(ChannelId new_channel) { mixer_controls.channel = new_channel; }

ChannelId Track::get_buffer_channel() const { return mixer_controls.buffer_channel; }
void Track::set_buffer_channel(ChannelId new_channel) { mixer_controls.buffer_channel = new_channel; }

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
