#include "audio/AudioFileStream.h"
#include "audio/Track.h"
#include "audio/StreamManager.h"

// Called by stream manager thread

AudioFileStream::AudioFileStream(int num_channels_, int total_buffer_size, juce::AudioFormatManager& format_manager_, Clip& clip_, StreamManager& stream_manager_)
    :   format_manager(format_manager_),
        stream_manager(stream_manager_),
        clip(clip_),
        file(clip_.file),
        reader(nullptr),
        buffer(num_channels_, total_buffer_size),
        refill_threshold(static_cast<int>(total_buffer_size * 0.7)) {
    initialize();
}

AudioFileStream::~AudioFileStream() {
    if (temp_buffer) {
        for (int i = 0; i < num_channels; ++i) {
            delete[] temp_buffer[i];
        }
        delete[] temp_buffer;
    }
    if (reader) {
        delete reader;
    }
}

void AudioFileStream::initialize() {
    if (reader) { return; }
    reader = format_manager.createReaderFor(file);
    num_channels = static_cast<int>(reader->numChannels);
    if (num_channels == 0 || num_channels > 2) {
        DBG("Only mono and stereo files are supported");
    }

    temp_buffer = new float*[static_cast<size_t>(num_channels)];
    for (int i = 0; i < num_channels; ++i) {
        temp_buffer[i] = new float[static_cast<size_t>(buffer.capacity)];
    }
}

bool AudioFileStream::is_done() {
    return stream_position >= clip.end;
}

bool AudioFileStream::is_initial_fill_queued() {
    return initial_fill_queued;
}

void AudioFileStream::set_initial_fill_queued(bool queued) {
    initial_fill_queued = queued;
}

// Called by worker thread

void AudioFileStream::fill() {
    // TODO: change ring buffer interface to allow directly writing to buffer
    // also deal case where track and file have different number of channels
    if (is_done()) { return; }

    int num_samples = buffer.getFreeSpace();
    if (!reader->read(temp_buffer, num_channels, stream_position, num_samples)) {
        DBG("Stream read failed: " + file.getFileName());
        return;
    }
    buffer.write(temp_buffer, num_samples);
    stream_position += num_samples;

    refill_requested.store(false);
    refill_queued.store(false);
}

int AudioFileStream::get_buffer_free_space() const {
    return buffer.getFreeSpace();
}

// Called by audio thread

bool AudioFileStream::check_refill() const {
    return buffer.getNumReadySamples() < refill_threshold;
}

void AudioFileStream::check_and_queue_refill() {
    if (refill_requested.load() || buffer.getNumReadySamples() >= refill_threshold) {
        return;
    }
    refill_requested.store(true);
    if (stream_manager.try_queue_refill(&clip)) {
        refill_queued.store(true);
    }
}

int AudioFileStream::read(float* const* data, int num_samples) {
    num_samples = buffer.read(data, num_samples);
    return num_samples;
}

AudioFileStreamFill::AudioFileStreamFill(Clip& clip_): juce::ThreadPoolJob("Fill: " + clip_.file.getFileName()), clip(clip_) {}

juce::ThreadPoolJob::JobStatus AudioFileStreamFill::runJob() {
    if (clip.stream) {
        clip.stream->fill();
        clip.stream->fill_in_progress.store(false);
    }
    return juce::ThreadPoolJob::JobStatus::jobHasFinished;
}
