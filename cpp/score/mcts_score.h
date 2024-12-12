// mcts_random.h
#pragma once
#include "env2048.h"

class MCTSScore {
public:
    MCTSScore(int n, int simulations, double c_param);
    bool makeMove();  // Returns true if game is over
    int getPoints() const { return points; }
    const Game2048& getGame() const { return game; }

private:
    int moveToEnd(int move);
    Game2048 game;
    int simulations;
    int points;
};