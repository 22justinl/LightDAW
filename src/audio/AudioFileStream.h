#pragma once

#include "Types.h"
#include "audio/AudioRingBuffer.h"

#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>

#include <atomic>

struct Clip;
class StreamManager;

class AudioFileStream {
public:
    AudioFileStream(int num_channels_, int total_buffer_size, juce::AudioFormatManager& format_manager_, Clip& clip_, StreamManager& stream_manager_);
    ~AudioFileStream();
    void initialize();
    bool is_done();
    bool is_initial_fill_queued();
    void set_initial_fill_queued(bool queued);

    void fill();

    int get_buffer_free_space() const;

    bool check_refill() const;
    void check_and_queue_refill();
    int read(float* const* data, int num_samples);

    std::atomic_bool fill_in_progress{false};
private:
    juce::AudioFormatManager& format_manager;
    StreamManager& stream_manager;
    Clip& clip;

    juce::File file;
    juce::AudioFormatReader* reader;
    AudioRingBuffer buffer;
    int refill_threshold;
    int num_channels;

    SamplePosition stream_position = 0;
    bool initial_fill_queued = false;

    float** temp_buffer;
    std::atomic_bool refill_requested{true};
    std::atomic_bool refill_queued{true};
};

class AudioFileStreamFill: public juce::ThreadPoolJob {
public:
    AudioFileStreamFill(Clip& clip_);
    JobStatus runJob() override;
private:
    Clip& clip;
};
