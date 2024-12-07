// mcts_random.h
#pragma once
#include <map>
#include "env2048.h"

class MCTSExploreScore {
public:
    double C = 0.3;
    int T = 5;

    MCTSExploreScore(int n, int simulations);
    std::string getBoardString(Game2048 board);
    bool makeMove();  // Returns true if game is over
    int getPoints() const { return points; }
    const Game2048& getGame() const { return game; }

private:
    int moveToEnd(std::map<std::string, double> &scores, std::map<std::string, double> &visits);
    Game2048 game;
    int simulations;
    int points;
};