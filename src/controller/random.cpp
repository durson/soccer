#include "include/board.hpp"
#include <iostream>
#include <unistd.h>

extern "C" {
    void* init() {
        return nullptr;
    }

    int heur(Board& board) {
        return W/2 - board.idx_x(board.get_ball());
    }
    
    int negamax(Board& board, dir_t* bd_ptr, int depth = 0) {
        static int max_depth = 9;
        bool rturn = board.get_rturn();
        int sign = rturn ? 1 : -1;
        if (board.get_state() == LWIN) {
            *bd_ptr = dir_invalid;
            return sign * -99;
        }
        if (board.get_state() == RWIN) {
            *bd_ptr = dir_invalid;
            return sign * +99;
        }
        if (depth == max_depth) {
            *bd_ptr = rand() % 8;
            return sign * heur(board);
        }
        int bv = -100;
        dir_t bd = dir_invalid;
        for (dir_t d = 0; d < 8; d++) {
            if (board.is_move_forbidden(d)) continue;
            board.do_move(d);
            int sign = (rturn ^ board.get_rturn()) ? -1 : 1;
            dir_t bd_opp;
            int new_val = sign * negamax(board, &bd_opp, depth + 1);
            if (depth == 0) {
                //std::cout << int(d) << ": " << new_val << std::endl;
            }
            if (new_val > bv) {
                bv = new_val;
                bd = d;
            }
            board.undo_move();
        }
        *bd_ptr = bd;
        return bv;
    }
    
    dir_t gen_move(void* _ctx, Board& board) {
        dir_t bd;
        int score = negamax(board, &bd);
        std::cout << score << std::endl;
        return bd;
    }

    void destroy(void* _ctx) {
    }
}
