#include "mcts_random.h"

int main() {
    MCTSRandom mcts(1, 100);  // 1 board, 100 simulations per move
    while (!mcts.makeMove()) {
        // Game continues
    }
    return 0;
}