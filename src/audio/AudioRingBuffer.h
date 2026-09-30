#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

class AudioRingBuffer {
public:
    AudioRingBuffer(int num_channels, int capacity);
    int getNumReadySamples() const;
    int getFreeSpace() const;

    int write(const float* const* data, int num_samples);
    int read(float* const* data, int num_samples);

    void clear();

    const int capacity;
private:
    juce::AudioBuffer<float> buffer;
    juce::AbstractFifo fifo;
};
