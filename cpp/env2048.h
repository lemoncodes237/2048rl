// env2048.h
#pragma once
#include <vector>
#include <random>

class Game2048 {
public:
    Game2048(int numBoards);
    Game2048(const Game2048& other);  // Copy constructor for MCTS

    int numBoards;
    
    struct MoveResult {
        bool gameOver;
        bool changed;
        int reward;
        int merges;
    };

    MoveResult move(int direction);  // 0=Up, 1=Down, 2=Right, 3=Left
    MoveResult moveWithoutSpawn(int direction);
    bool isGameOver(const std::vector<int>& board) const;
    const std::vector<std::vector<int>>& getBoards() const { return boards; }

    std::vector<std::vector<int>> boards;

private:
    void genRandom(std::vector<int>& board);
    std::vector<int> moveLine(const std::vector<int>& line, bool& moved, int& score) const;

    std::mt19937 rng;
};