// mcts_random.h
#pragma once
#include "env2048.h"

class pUCTNode {
public:
    pUCTNode(unsigned long state, bool chance, int a);
    std::vector<pUCTNode*> children;
    double value;
    double visits;
    bool chance;
    // action is for chance node only
    int action;
    unsigned long state;

    void incrementVisits();
    void increaseValue(double v);
};

class MCTSpUCT {
public:
    double C = 1000;

    MCTSpUCT(int n, int simulations);
    unsigned long getBoardNum(Game2048* currGame);
    int selectAction(pUCTNode* node);
    double sample(pUCTNode* node, Game2048* currGame);
    void clearTree(pUCTNode* node, bool skipDelete);
    bool makeMove();  // Returns true if game is over
    int getPoints() const { return points; }
    const Game2048& getGame() const { return game; }

private:
    int moveToEnd(Game2048* currGame);
    Game2048 game;
    int simulations;
    int points;
    int acquired;
};