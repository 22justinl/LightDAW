#include "audio/Transport.h"
 
Transport::Transport() {

}

void Transport::play() {

}

void Transport::stop() {

}

bool Transport::is_playing() const {
    return playing.load();
}

int Transport::get_position() const {
    return position.load();
}
