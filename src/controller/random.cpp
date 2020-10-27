#include "include/board.hpp"
#include <iostream>
#include <unistd.h>

extern "C" {
    void* init() {
        return nullptr;
    }

    dir_t gen_move(void* _ctx, Board& board) {
        dir_t dir = dir_invalid;
        do {
            dir = rand() % 8;
        } while(board.is_move_forbidden(dir));
        usleep(1000 * 1000);
        return dir;
    }

    void destroy(void* _ctx) {
    }
}
