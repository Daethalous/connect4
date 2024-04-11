#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "Point.h"

using namespace std;

// 定义游戏状态结构体
class GameState
{
public:
    int M;       // 行数
    int N;       // 列数
    int **board; // 棋盘
    int *top;    // 列顶位置
    int player;  // 当前玩家
    GameState(const int M, const int N, const int *top, int **board, int player);
    GameState(const GameState &state);
    ~GameState();
};

GameState::GameState(const int M, const int N, const int *top, int **board, int player)
{
    this->M = M;
    this->N = N;
    this->player = player;
    this->board = new int *[M];
    for (int i = 0; i < M; i++)
    {
        this->board[i] = new int[N];
        for (int j = 0; j < N; j++)
        {
            this->board[i][j] = board[i][j];
        }
    }

    this->top = new int[N];
    for (int i = 0; i < N; i++)
    {
        this->top[i] = top[i];
    }
}

GameState::GameState(const GameState &state)
{
    this->M = state.M;
    this->N = state.N;
    this->player = state.player;
    this->board = new int *[M];
    for (int i = 0; i < M; i++)
    {
        this->board[i] = new int[N];
        for (int j = 0; j < N; j++)
        {
            this->board[i][j] = state.board[i][j];
        }
    }

    this->top = new int[N];
    for (int i = 0; i < N; i++)
    {
        this->top[i] = state.top[i];
    }
}

GameState::~GameState()
{
    for (int i = 0; i < this->M; i++)
    {
        delete[] this->board[i];
    }
    delete[] this->board;

    delete[] this->top;
}