#include "include/board.hpp"
#include <iostream>

extern "C" {
    void* init() {
        return nullptr;
    }

    dir_t gen_move(void* _ctx, Board& board) {
        std::string cmd;
        dir_t dir = dir_invalid;
    
        while(dir == dir_invalid) {
            if (std::cin.eof()) {
                exit(0);
            }
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore();
            }
            std::cout << (board.Rturn() ? "> " : "< ");
            getline(std::cin, cmd);
            if (cmd == "q") dir = UL;
            if (cmd == "w") dir = U;
            if (cmd == "e") dir = UR;
            if (cmd == "a") dir = L;
            if (cmd == "d") dir = R;
            if (cmd == "z") dir = DL;
            if (cmd == "s" || cmd == "x") dir = D;
            if (cmd == "c") dir = DR;
        
            if (cmd == "!") {
                board.undo_move();
                board.undo_move();
                continue;
            }
        
            if (dir == dir_invalid) {
                std::cout << "Invalid direction\n";
            } else if (board.is_move_forbidden(dir)) {
                std::cout << "Forbidden move\n";
                dir = dir_invalid;
            }
        }
        std::cout << dir;
        return dir;
    }

    void destroy(void* _ctx) {
    }
}
