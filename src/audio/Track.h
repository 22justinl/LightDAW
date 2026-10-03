#pragma once

#include "audio/AudioFileStream.h"
#include "audio/TrackAudioSource.h"
#include "audio/Transport.h"
#include "Types.h"

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_formats/juce_audio_formats.h>

struct Clip {
    Clip(ClipId id_, const juce::File& file_, SamplePosition pos_, SamplePosition start_, SamplePosition end_)
        : id(id_), file(file_), pos(pos_), start(start_), end(end_), end_pos(pos+end-start), stream(nullptr) {}
    ClipId id;
    juce::File file;
    SamplePosition pos;
    SamplePosition start;
    SamplePosition end;
    SamplePosition end_pos;
    std::unique_ptr<AudioFileStream> stream;
};

enum class ClipChangeType {
    Add,
    Delete,
    Edit,
    None
};

struct ClipChange {
    ClipChange(ClipId id_, ClipChangeType type_): id(id_), type(type_) { }
    ClipId id;
    ClipChangeType type;
};

struct TrackPlaybackState {
    size_t curr_clip = 0;       // clip at play position
    size_t next_clip = 0;       // next clip to create stream for
    size_t prefetch_clip = 0;   // next clip to prefetch
};

class Track: public juce::ChangeBroadcaster {
public:
    Track(Transport& transport_, TrackId id_);
    Track(Transport& transport_, TrackId id_, const juce::String& name);

    ~Track();

    const juce::String& get_name() const;
    void set_name(const juce::String& new_name);

    float get_gain() const;
    void set_gain(float new_gain_db);

    bool is_muted() const;
    void set_muted(bool new_muted);

    bool is_monitoring() const;
    void set_monitoring(bool new_monitoring);

    const juce::String& get_selected_device_name() const;
    void set_selected_device_name(const juce::String& new_device_name);

    ChannelId get_selected_channel() const;
    void set_selected_channel(ChannelId new_channel);

    ChannelId get_channel() const;
    void set_channel(ChannelId new_channel);

    ChannelId get_buffer_channel() const;
    void set_buffer_channel(ChannelId new_channel);

    TrackAudioSource& get_audio_source();

    SamplePosition get_max_sample_position() const;

    const Clip* get_clip_by_id(ClipId clip_id) const;

    std::queue<ClipChange> clip_change_queue;

    void import_file(const juce::File& file, SamplePosition pos, SamplePosition start, SamplePosition end);

    std::vector<Clip> clips; // sorted by start position

    TrackPlaybackState playback_state;
private:
    Transport& transport;

    const TrackId id;
    juce::String name = "New Track";
    ClipId next_clip_id = 0;

    float gain_db = 0;
    bool muted = false;
    bool monitoring = false;

    // TODO: stereo/mono and panning (juce::dsp::Panner)
    // mono also needs panning (to center) not just copies on left and right

    juce::String selected_device_name = "";
    ChannelId selected_channel = -1;
    ChannelId channel = -1;         // use this if current device is not selected device
    ChannelId buffer_channel = -1;  // channel index in callback buffer

    TrackAudioSource audio_source;
};
