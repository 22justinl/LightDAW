#pragma once

#include "Types.h"
#include <atomic>

class Transport {
public:
    Transport();
    void play();
    void stop();

    bool is_playing() const;
    SamplePosition get_position() const;
    void set_position(SamplePosition new_position);

    SamplePosition get_end_pos();
    void set_end_pos(SamplePosition new_end_pos);
private:
    std::atomic_bool playing{false};
    std::atomic<SamplePosition> position{0};

    SamplePosition end_pos = INT_MAX;
};
