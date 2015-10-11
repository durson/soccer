#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <time.h>
#include <cassert>


using namespace std;


typedef unsigned int  uint;
typedef unsigned char uchar;

const uchar w = 11;
const uchar h = 9;
const uchar h0 = 3;
const uchar h1 = 5;

uchar board[w*h];
uchar ball;
bool  rightPlayerTurn;

uchar flag[] =
{
    1 << 0,
    1 << 1,
    1 << 2,
    1 << 3,
    1 << 4,
    1 << 5,
    1 << 6,
    1 << 7
};

char dind[] =
{
    w-1,
    w,
    w+1,
    -1,
    1,
    -w-1,
    -w,
    -w+1
};

//string label[] =
//{
//    "down left",
//    "down",
//    "down right",
//    "left",
//    "right",
//    "up left",
//    "up",
//    "up right"
//};

string label[] =
{
    "up left",
    "left",
    "down left",
    "up",
    "down",
    "up right",
    "right",
    "down right"
};

const uchar NW = 0;
const uchar N  = 1;
const uchar NE = 2;
const uchar W  = 3;
const uchar E  = 4;
const uchar SW = 5;
const uchar S  = 6;
const uchar SE = 7;

const uchar blocked = flag[W] | flag[SW] | flag[S] | flag[SE] | flag[E] | flag[NW] | flag[N] | flag[NE];

enum GSTATE
{
    RUN = 0,
    LWIN = 1,
    RWIN = 2
};

GSTATE state;

const uint maxNumMoves = 305;
uint  numMoves;
uchar moveHistory[maxNumMoves];
bool  playerTurnHistory[maxNumMoves];

uchar cdir(uchar dir)
{
    return 7-dir;
}

uchar ind(uchar x, uchar y)
{
    return x + w*y;
}

const uchar wc0 = ind(0,   h/2);
const uchar wc1 = ind(w-1, h/2);

uchar indX(uchar i)
{
    return i % w;
}

uchar indY(uchar i)
{
    return i / w;
}

void setupBoard()
{
    memset(board, 0, sizeof(board));

    for(uint i = 0u; i < w; ++i)
    {
        board[ind(i, 0)  ] |= flag[W] | flag[SW] | flag[S] | flag[SE] | flag[E];
        board[ind(i, h-1)] |= flag[W] | flag[NW] | flag[N] | flag[NE] | flag[E];
    }

    for(uint i = 0u; i < h; ++i)
    {
        board[ind(0,   i)] |= flag[S] | flag[SW] | flag[W] | flag[NW] | flag[N];
        board[ind(w-1, i)] |= flag[S] | flag[SE] | flag[E] | flag[NE] | flag[N];
    }

    ball = ind(w/2, h/2);
    rightPlayerTurn = true;
    state = RUN;
    numMoves = 0;
}

void makeMove(uchar dir)
{
    moveHistory[numMoves] = dir;
    playerTurnHistory[numMoves] = rightPlayerTurn;
    ++numMoves;

    board[ball] |= flag[dir];
    ball += dind[dir];

    uchar ballX = indX(ball);
    uchar ballY = indY(ball);

    if(((!ballX && rightPlayerTurn) || (ballX == w-1 && !rightPlayerTurn)) && (ballY == h0 || ballY == h1))
    {
        if(!ballX)
            state = RWIN;
        else
            state = LWIN;
    }

    if((!board[ball] && ballX && ballY && ballX != w-1 && ballY != h-1) || ball == wc0 || ball == wc1)
        rightPlayerTurn = !rightPlayerTurn;

    board[ball] |= flag[cdir(dir)];

    if(board[ball] == blocked)
    {
        if(!rightPlayerTurn)
            state = RWIN;
        else
            state = LWIN;
    }
}

void undoMove()
{
    uchar dir = moveHistory[--numMoves];
    rightPlayerTurn = playerTurnHistory[numMoves];

    board[ball] &= ~flag[cdir(dir)];
    ball += dind[cdir(dir)];
    board[ball] &= ~flag[dir];

    state = RUN;
}

const int inf = 100;

int evalR()
{
    return -indX(ball);
}

int evalL()
{
    return -w+1+indX(ball);
}

int eval()
{
    if(state == LWIN)
        return inf-1;
    else if(state == RWIN)
        return -inf+1;
    else
        return evalL() - evalR();
}

int negamax(int depth, uchar* bestMove, int A = -inf, int B = inf)
{
    if(!depth || (int)state)
    {
        return eval() * (rightPlayerTurn ? -1 : 1);
    }

    int best = -inf;

    for(uchar i = 0; i < 8; ++i)
    {
        if(!(board[ball] & flag[i]))
        {
            makeMove(i);

            bool turnChanged = (playerTurnHistory[numMoves-1] != rightPlayerTurn);

            int Anew = (turnChanged ? -B : A);
            int Bnew = (turnChanged ? -A : B);
            int val = negamax(depth-1, nullptr, Anew, Bnew) * (turnChanged ? -1 : 1);

            undoMove();

            if(val >= B)
            {
                //if(bestMove)
                //    cout << "\nCut-off " << label[i] << " score:" << val << " beta:" << B;
                return val;
            }

            if(val > best)
            {
                best = val;
                if(val > A)
                    A = val;
                if(bestMove)
                    *bestMove = i;
            }
        }
    }

    return best;
}

uchar computerMove()
{
    uchar bestMove = 8;
    negamax(20, &bestMove, -inf, 1);
    assert(bestMove != 8);

    return bestMove;
}

int main()
{
    uint numGames = 0;
    while(true)
    {
        ++numGames;
        cout << "Game " << numGames << '\n';

        setupBoard();

        while(!(int)state)
        {
            if(!rightPlayerTurn)
            {
                uchar dir = computerMove();
                makeMove(dir);

                cout << numMoves << "........... " << (playerTurnHistory[numMoves-1] ? "R" : "L") << " " << label[dir] << '\n';
            }
            else
            {
                string command;
                uchar dir = 8;

                while(dir == 8)
                {
                    cout << ": ";
                    getline(cin, command);
                    for(uchar i = 0; i < 8; ++i)
                    {
                        if(label[i] == command)
                        {
                            dir = i;
                            break;
                        }
                    }
                }

                makeMove(dir);

                cout << numMoves << "........... " << (playerTurnHistory[numMoves-1] ? "R" : "L") << " " << label[dir] << '\n';
            }
        }

        if(state == LWIN)
            cout << "Left player won!\n\n";
        else
            cout << "Right player won!\n\n";

        break;
    }

    return 0;
}
