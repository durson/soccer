#ifndef BOARD_HPP
#define BOARD_HPP

#include <cstdint>

class Controller;

typedef uint8_t dir_t;
typedef int32_t idx_t;

const dir_t UL = 0;
const dir_t U  = 1;
const dir_t UR = 2;
const dir_t L  = 3;
const dir_t R  = 4;
const dir_t DL = 5;
const dir_t D  = 6;
const dir_t DR = 7;
const dir_t dir_invalid = 8;

enum GSTATE {
    PLAYING = 0,
    LWIN = 1,
    RWIN = 2
};

#define W 11
#define H 7

class Board {
public:
    Board();
    ~Board();

    dir_t* get_board();
    idx_t get_ball();
    bool get_rturn();
    GSTATE get_state();
    
    bool is_move_forbidden(dir_t move);
    void do_move(dir_t dir);
    void undo_move();

    GSTATE play();
    void display();

    idx_t didx(dir_t i);
    idx_t idx_x(idx_t i);
    idx_t idx_y(idx_t i);
    idx_t idx(idx_t x, idx_t y);
    dir_t dir_rev(dir_t dir);
    dir_t blckd(dir_t i);

    void set_l_controller(Controller* c);
    void set_r_controller(Controller* c);

private:
    const static uint32_t max_moves = 305;

    dir_t board[(W+2)*H];
    idx_t ball;
    bool rturn;
    GSTATE state;
    uint32_t num_moves;
    idx_t move_history[max_moves];
    bool turn_history[max_moves];

    Controller* l_controller;
    Controller* r_controller;
};

#endif // BOARD_HPP
