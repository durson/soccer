#include <iostream>
#include <cstring>
#include <string>
#include <cassert>

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
const dir_t field_blocked = ~0;

enum GSTATE {
    PLAYING = 0,
    LWIN = 1,
    RWIN = 2
};

template<idx_t W = 11, idx_t H = 9, bool rstart = true>
class Board {
public:
    Board() {
        real_w = W+2;
        real_h = H;
        
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
        rturn = rstart;
        state = PLAYING;
        num_moves = 0;
    }

    ~Board() {}

    bool move_forbidden(dir_t move) {
        return board[ball] & blckd(move);
    }

    void make_move(dir_t dir) {
        if (move_forbidden(dir))
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
        else if(board[ball] == 0)
            rturn = !rturn;
        
        board[ball] |= blckd(cdir(dir));
        
        if(board[ball] == field_blocked)
            state = rturn ? LWIN : RWIN;
    }

    void undo_move() {
        if (num_moves == 0)
            return;
        
        dir_t dir = move_history[--num_moves];
        rturn = turn_history[num_moves];
        
        board[ball] &= ~blckd(cdir(dir));
        ball += didx(cdir(dir));
        board[ball] &= ~blckd(dir);
        
        state = PLAYING;
    }

    dir_t gen_move_human() {
        std::string command;
        dir_t dir = dir_invalid;
            
        while(dir == dir_invalid) {
            if (std::cin.eof()) {
                exit(0);
            }
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore();
            }
            std::cout << (rturn ? "> " : "< ");
            getline(std::cin, command);
            for(dir_t i = 0; i < 8; ++i) {
                if(label(i) == command) {
                    dir = i;
                    break;
                }
            }
            if (dir == dir_invalid) {
                std::cout << "Invalid direction\n";
            } else if (move_forbidden(dir)) {
                std::cout << "Forbidden move\n";
                dir = dir_invalid;
            }
        }
        return dir;
    }

    dir_t gen_move_rng() {
        dir_t dir = dir_invalid;
        do {
            dir = rand() % 8;
        } while(move_forbidden(dir));
        return dir;
    }
    
    GSTATE play() {
        while(state == PLAYING) {
            display();
            dir_t dir = dir_invalid;
            if (rturn) dir = gen_move_human();
            else       dir = gen_move_rng();
            assert(!move_forbidden(dir));
            make_move(dir);
        }
        return state;
    }

    void display() {
        char ball_sym = (rturn ? '<' : '>');
        for (idx_t y = H-1; y >= 0; y--) {
            for (idx_t x = 1; x <= W; x++) {
                std::cout << (idx(x, y) == ball ? ball_sym : '.');
                if (x != W) std::cout << (board[idx(x, y)] & blckd(R) ? "__" : "  ");
            }
            std::cout << "\n";
            if (y != 0) {
                for (idx_t x = 1; x <= W; x++) {
                    std::cout << (board[idx(x,   y)] & blckd(D) ? "|" : " ");
                    if (x != W) std::cout << (board[idx(x,   y)] & blckd(DR) ? "\\" : " ");
                    if (x != W) std::cout << (board[idx(x+1, y)] & blckd(DL) ? "/" : " ");
                }
                std::cout << "\n";
                for (idx_t x = 1; x <= W; x++) {
                    std::cout << (board[idx(x,   y)] & blckd(D) ? "|" : " ");
                    if (x != W) std::cout << (board[idx(x+1, y)] & blckd(DL) ? "/" : " ");
                    if (x != W) std::cout << (board[idx(x,   y)] & blckd(DR) ? "\\" : " ");
                }
                std::cout << "\n";
            }
        }
        std::cout.flush();
    }

private:
    const static uint32_t max_moves = 305;

    dir_t blckd(dir_t i) {
        return 1 << i;
    }

    idx_t didx(dir_t i) {
        static idx_t _didx[] = {
            real_w-1,
            real_w,
            real_w+1,
            -1,
            1,
            -real_w-1,
            -real_w,
            -real_w+1
        };
        return _didx[i];
    }

    std::string label(dir_t i) {
        std::string _label[] = {
            "q",
            "w",
            "e",
            "a",
            "d",
            "z",
            "x",
            "c"
        };
        return _label[i];
    }

    idx_t idx_x(idx_t i) {
        return i % real_w;
    }

    idx_t idx_y(idx_t i) {
        return i / real_w;
    }

    dir_t cdir(dir_t dir) {
        return 7 - dir;
    }

    idx_t idx(idx_t x, idx_t y) {
        return x + real_w*y;
    }

    idx_t real_w;
    idx_t real_h;
    dir_t board[(W+2)*H];
    idx_t ball;
    bool rturn;    
    GSTATE state;
    uint32_t num_moves;
    idx_t move_history[max_moves];
    bool turn_history[max_moves];
};

int main() {
    uint numGames = 0;
    while(true) {
        ++numGames;
        std::cout << "Game " << numGames << '\n';

        Board<> board;
        GSTATE state = board.play();

        if(state == LWIN)
            std::cout << "Left player won!\n\n";
        else
            std::cout << "Right player won!\n\n";
        break;
    }

    return 0;
}
