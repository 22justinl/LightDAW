#pragma once

#include <atomic>
class Transport {
public:
    Transport();
    void play();
    void stop();

    bool is_playing() const;
    int get_position() const;
private:
    std::atomic_bool playing{false};
    std::atomic_int position{0};
};
