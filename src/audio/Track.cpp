#include "audio/Track.h"
#include "juce_core/juce_core.h"

// PUBLIC FUNCTIONS

Track::Track(Transport& transport_, TrackId id_): transport(transport_), id(id_), name("Track " + std::to_string(id_)), audio_source(transport_, clips) { }
Track::Track(Transport& transport_, TrackId id_, const juce::String& name_): transport(transport_), id(id_), name(name_), audio_source(transport_, clips) { }

Track::~Track() {
    for (auto& c : clips) {
        c.stream = nullptr;
    }
}

const juce::String& Track::get_name() const { return name; }
void Track::set_name(const juce::String& new_name) { name = new_name; }

float Track::get_gain() const { return gain_db; }
void Track::set_gain(float new_gain_db) { gain_db = new_gain_db; }

bool Track::is_muted() const { return muted; }
void Track::set_muted(bool new_muted) { muted = new_muted; }

bool Track::is_monitoring() const { return monitoring; }
void Track::set_monitoring(bool new_monitoring) { monitoring = new_monitoring; }

const juce::String& Track::get_selected_device_name() const { return selected_device_name; }
void Track::set_selected_device_name(const juce::String& new_device_name) { selected_device_name = new_device_name; }

ChannelId Track::get_selected_channel() const { return selected_channel; }
void Track::set_selected_channel(ChannelId new_channel) { selected_channel = new_channel; }

ChannelId Track::get_channel() const { return channel; }
void Track::set_channel(ChannelId new_channel) { channel = new_channel; }

ChannelId Track::get_buffer_channel() const { return buffer_channel; }
void Track::set_buffer_channel(ChannelId new_channel) { buffer_channel = new_channel; }

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
