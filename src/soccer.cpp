#include "include/board.hpp"
#include "include/controller.hpp"

int main(int argv, char** argc) {
    if (argv != 3) {
        std::cerr << "Usage: ./soccer [left_controller.so] [right_controller.so]" << std::endl;
        exit(1);
    }
    
    uint numGames = 0;
    while(true) {
        ++numGames;
        std::cout << "Game " << numGames << '\n';

        Board board;
        Controller l_controller(argc[1]);
        Controller r_controller(argc[2]);

        board.set_l_controller(&l_controller);
        board.set_r_controller(&r_controller);
        GSTATE state = board.play();

        if(state == LWIN)
            std::cout << "Left player won!\n\n";
        else
            std::cout << "Right player won!\n\n";
        break;
    }

    return 0;
}
