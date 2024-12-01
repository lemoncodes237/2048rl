// mcts_random.h
#pragma once
#include "env2048.h"

class MCTSRandom {
public:
    MCTSRandom(int n, int simulations);
    bool makeMove();  // Returns true if game is over
    int getPoints() const { return points; }
    const Game2048& getGame() const { return game; }

private:
    int randomToEnd(int move);
    Game2048 game;
    int simulations;
    int points;
};