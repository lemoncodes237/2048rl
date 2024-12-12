// mcts_random.h
#pragma once
#include "env2048.h"

class pUCTNode {
public:
    pUCTNode(std::vector<unsigned long> state, bool chance, int a);
    std::vector<pUCTNode*> children;
    double value;
    double visits;
    bool chance;
    // action is for chance node only
    int action;
    std::vector<unsigned long> state;

    void incrementVisits();
    void increaseValue(double v);
    bool matches(std::vector<unsigned long> state2);
};

class MCTSpUCT {
public:
    double C;

    MCTSpUCT(int n, int simulations, double c_param = 800.0);  // Added C parameter with default
    unsigned long getBoardNum(Game2048* currGame, int gameNum);
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