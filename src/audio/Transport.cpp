#include "audio/Transport.h"
 
Transport::Transport() { }

void Transport::play() {
    position.store(0);
    playing.store(true);

}

void Transport::stop() {
    playing.store(false);
}

bool Transport::is_playing() const {
    return playing.load();
}

SamplePosition Transport::get_position() const {
    return position.load();
}

void Transport::set_position(SamplePosition new_position) {
    position.store(new_position);
}

SamplePosition Transport::get_end_pos() {
    return end_pos;
}

void Transport::set_end_pos(SamplePosition new_end_pos) {
    end_pos = new_end_pos;
}
