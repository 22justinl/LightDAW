#pragma once

#include <juce_core/juce_core.h>

#include <vector>

template <typename T>
class SPSCQueue {
public:
    SPSCQueue(size_t capacity);

    bool push(T val);
    bool pop(T& val);

    bool empty() const;
    bool full() const;

    void clear();
private:
    std::vector<T> buffer;
    juce::AbstractFifo fifo;
};

template <typename T>
SPSCQueue<T>::SPSCQueue(size_t capacity): buffer(capacity+1), fifo(static_cast<int>(capacity+1)) {}

template <typename T>
bool SPSCQueue<T>::push(T val) {
    int start1, size1, start2, size2;
    fifo.prepareToWrite(1, start1, size1, start2, size2);
    if (size1 > 0) {
        buffer[static_cast<size_t>(start1)] = val;
        fifo.finishedWrite(1);
        return true;
    } else if (size2 > 0) {
        buffer[static_cast<size_t>(start2)] = val;
        fifo.finishedWrite(1);
        return true;
    }
    return false;
}

template <typename T>
bool SPSCQueue<T>::pop(T& val) {
    int start1, size1, start2, size2;
    fifo.prepareToRead(1, start1, size1, start2, size2);
    if (size1 > 0) {
        val = buffer[static_cast<size_t>(start1)];
        fifo.finishedRead(1);
        return true;
    } else if (size2 > 0) {
        val = buffer[static_cast<size_t>(start2)];
        fifo.finishedRead(1);
        return true;
    }
    return false;
}

template <typename T>
bool SPSCQueue<T>::empty() const {
    return fifo.getNumReady() == 0;
}

template <typename T>
bool SPSCQueue<T>::full() const {
    return fifo.getFreeSpace() == 0;
}

template <typename T>
void SPSCQueue<T>::clear() {
    fifo.reset();
}
