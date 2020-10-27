#include "include/board.hpp"
#include <vector>
#include <iostream>
#include <unistd.h>

extern "C" {
    struct State {
        int lvl;
    };
    
    void* init(int lvl) {
        return (void*) new State{lvl};
    }

    int heur(Board& board) {
        return W/2 - board.idx_x(board.get_ball());
    }
    
    int negamax(Board& board, int depth, dir_t* bd_ptr, int A = -100, int B = 100) {
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
        if (depth == 0) {
            *bd_ptr = rand() % 8;
            return sign * heur(board);
        }
        int bv = -100;
        dir_t bd = dir_invalid;
        for (dir_t d = 0; d < 8; d++) {
            if (board.is_move_forbidden(d)) continue;
            board.do_move(d);
            bool player_changed = rturn ^ board.get_rturn();
            int sign = (player_changed ? -1 : 1);
            int new_A = (player_changed ? -B : A);
            int new_B = (player_changed ? -A : B);
            dir_t bd_opp;
            int new_val = sign * negamax(board, depth - 1, &bd_opp, new_A, new_B);
            if (new_val > bv) {
                bv = new_val;
                bd = d;
            }
            board.undo_move();
            if (new_val > A)
                A = new_val;
            if (A >= B)
                break;
        }
        *bd_ptr = bd;
        return bv;
    }
    
    dir_t gen_move(void* ctx, Board& board) {
        State* state = (State*) ctx;
        std::vector<int> levels = {1, 2, 3, 4, 6, 8, 10, 12, 14, 15, 16};
        int depth = levels[state->lvl];
        dir_t bd;
        int score = negamax(board, depth, &bd);
        std::cout << score << std::endl;
        return bd;
    }

    void destroy(void* ctx) {
        delete (State*) ctx;
    }
}
