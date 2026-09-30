#include "audio/AudioRingBuffer.h"

AudioRingBuffer::AudioRingBuffer(int num_channels, int capacity_): capacity(capacity_), buffer(num_channels, capacity_+1), fifo(capacity_+1) { }

int AudioRingBuffer::getNumReadySamples() const {
    return fifo.getNumReady();
}

int AudioRingBuffer::getFreeSpace() const {
    return fifo.getFreeSpace();
}

int AudioRingBuffer::write(const float* const* data, int num_samples) {
    int start1, size1, start2, size2;
    fifo.prepareToWrite(num_samples, start1, size1, start2, size2);
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        if (size1 > 0) {
            juce::FloatVectorOperations::copy(buffer.getWritePointer(channel, start1), data[channel], size1);
        }
        if (size2 > 0) {
            juce::FloatVectorOperations::copy(buffer.getWritePointer(channel, start2), data[channel]+size1, size2);
        }
    }
    fifo.finishedWrite(size1+size2);

    return size1+size2;
}

int AudioRingBuffer::read(float* const* data, int num_samples) {
    int start1, size1, start2, size2;
    fifo.prepareToRead(num_samples, start1, size1, start2, size2);
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        if (size1 > 0) {
            juce::FloatVectorOperations::copy(data[channel], buffer.getReadPointer(channel, start1), size1);
        }
        if (size2 > 0) {
            juce::FloatVectorOperations::copy(data[channel]+size1, buffer.getReadPointer(channel, start2), size2);
        }
    }
    fifo.finishedRead(size1+size2);

    return size1+size2;
}

// int AudioRingBuffer::write(int num_samples, float**& start1, int& size1, float**& start2, int& size2) {
//
// }

void AudioRingBuffer::clear() {
    fifo.reset();
}
