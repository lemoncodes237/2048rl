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
    double C = 800;

    MCTSpUCT(int n, int simulations);
    unsigned long getBoardNum();
    int selectAction(pUCTNode* node);
    double sample(pUCTNode* node);
    bool makeMove();  // Returns true if game is over
    int getPoints() const { return points; }
    const Game2048& getGame() const { return game; }

private:
    int randomToEnd();
    Game2048 game;
    // Simulated game
    Game2048* currGame;
    int simulations;
    int points;
    int acquired;
};