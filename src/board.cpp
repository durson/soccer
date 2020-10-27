#include "include/board.hpp"
#include "include/controller.hpp"

#include <iostream>
#include <cstring>
#include <string>
#include <cassert>

Board::Board() {
    memset(board, 0, sizeof(board));
    for(idx_t x = 1; x <= W; ++x) {
        board[idx(x, 0)  ] |= blckd(L) | blckd(DL) | blckd(D) | blckd(DR) | blckd(R);
        board[idx(x, H-1)] |= blckd(L) | blckd(UL) | blckd(U) | blckd(UR) | blckd(R);
    }
    
    for(idx_t y = 0; y < H; ++y) {
        board[idx(1, y)] |= blckd(D) | blckd(DL) | blckd(L) | blckd(UL) | blckd(U);
        board[idx(W, y)] |= blckd(D) | blckd(DR) | blckd(R) | blckd(UR) | blckd(U);
    }
    
    board[idx(1, H/2 - 1)] = blckd(D) | blckd(DL) | blckd(L);
    board[idx(1, H/2    )] = 0;
    board[idx(1, H/2 + 1)] = blckd(L) | blckd(UL) | blckd(U);
    
    board[idx(W, H/2 - 1)] = blckd(D) | blckd(DR) | blckd(R);
    board[idx(W, H/2    )] = 0;
    board[idx(W, H/2 + 1)] = blckd(R) | blckd(UR) | blckd(U);
    
    ball = idx(1 + W/2, H/2);
    rturn = true;
    state = PLAYING;
    num_moves = 0;
}

Board::~Board() {}

dir_t* Board::get_board() {
    return board;
}

idx_t Board::get_ball() {
    return ball;
}

bool Board::get_rturn() {
    return rturn;
}

GSTATE Board::get_state() {
    return state;
}

bool Board::is_move_forbidden(dir_t move) {
    return board[ball] & blckd(move);
}

void Board::do_move(dir_t dir) {
    if (is_move_forbidden(dir))
        return;
    
    move_history[num_moves] = dir;
    turn_history[num_moves] = rturn;
    ++num_moves;
    
    board[ball] |= blckd(dir);
    ball += didx(dir);
    
    idx_t ball_x = idx_x(ball);
    
    if (ball_x == 0)
        state = RWIN;
    else if (ball_x == W+1)
        state = LWIN;
    else if(board[ball] == dir_t(0))
        rturn = !rturn;
    
    board[ball] |= blckd(dir_rev(dir));

    // no moves
    if(board[ball] == dir_t(~0))
        state = rturn ? LWIN : RWIN;
}

void Board::undo_move() {
    if (num_moves == 0)
        return;

    --num_moves;
    dir_t dir = move_history[num_moves];
    rturn = turn_history[num_moves];
    
    board[ball] &= ~blckd(dir_rev(dir));
    ball -= didx(dir);
    board[ball] &= ~blckd(dir);
    
    state = PLAYING;
}

GSTATE Board::play() {
    while(state == PLAYING) {
        display();
        dir_t dir = dir_invalid;
        if (rturn) dir = r_controller->gen_move(*this);
        else       dir = l_controller->gen_move(*this);
        assert(!is_move_forbidden(dir));
        do_move(dir);
    }
    display();
    return state;
}

void Board::display() {
    std::string ball_sym = (rturn ? "•" : "•");
    std::string dot_sym = "·";
    std::string double_hsym = "──";
    std::string vsym = "│";
    std::string rsym = "╱";
    std::string lsym = "╲";
    for (idx_t y = H-1; y >= 0; y--) {
        std::cout << (y == H/2 ? dot_sym : " ") << "  ";
        for (idx_t x = 1; x <= W; x++) {
            std::string dot_or_box = dot_sym;
            bool top = (y == H-1);
            bool bottom = (y == 0);
            bool left = (x == 1);
            bool right = (x == W);
            if (top || bottom)    dot_or_box = "─";
            if (left || right)    dot_or_box = "│";
            if (top && left)      dot_or_box = "┌";
            if (top && right)     dot_or_box = "┐";
            if (bottom && left)   dot_or_box = "└";
            if (bottom && right)  dot_or_box = "┘";
            if (y == H/2)         dot_or_box = dot_sym;
            
            std::cout << (idx(x, y) == ball ? ball_sym : dot_or_box);
            if (x != W) std::cout << (board[idx(x, y)] & blckd(R) ? double_hsym : "  ");
        }
        std::cout << "  " << (y == H/2 ? dot_sym : " ");
        std::cout << "\n";
        if (y != 0) {
            std::cout << "   ";
            for (idx_t x = 1; x <= W; x++) {
                std::cout << (board[idx(x,   y)] & blckd(D) ? vsym : " ");
                if (x != W) std::cout << (board[idx(x,   y)] & blckd(DR) ? lsym : " ");
                if (x != W) std::cout << (board[idx(x+1, y)] & blckd(DL) ? rsym : " ");
            }
            std::cout << "\n";
            std::cout << "   ";
            for (idx_t x = 1; x <= W; x++) {
                std::cout << (board[idx(x,   y)] & blckd(D) ? vsym : " ");
                if (x != W) std::cout << (board[idx(x+1, y)] & blckd(DL) ? rsym : " ");
                if (x != W) std::cout << (board[idx(x,   y)] & blckd(DR) ? lsym : " ");
            }
            std::cout << "\n";
        }
    }
    std::cout.flush();
}

idx_t Board::didx(dir_t i) {
    static idx_t _didx[] = {
        (W+2)-1,
        (W+2),
        (W+2)+1,
        -1,
        1,
        -(W+2)-1,
        -(W+2),
        -(W+2)+1
    };
    return _didx[i];
}

idx_t Board::idx_x(idx_t i) {
    return i % (W+2);
}

idx_t Board::idx_y(idx_t i) {
    return i / (W+2);
}

idx_t Board::idx(idx_t x, idx_t y) {
    return x + (W+2)*y;
}

dir_t Board::dir_rev(dir_t dir) {
    return 7 - dir;
}

dir_t Board::blckd(dir_t i) {
    return 1 << i;
}

void Board::set_l_controller(Controller* c) {
    l_controller = c;
}

void Board::set_r_controller(Controller* c) {
    r_controller = c;
}
