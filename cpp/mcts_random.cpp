// mcts_random.cpp
#include "mcts_random.h"
#include <random>
#include <algorithm>
#include <vector>
#include <iostream>
#include <omp.h>

MCTSRandom::MCTSRandom(int n, int simulations) 
    : game(n), simulations(simulations), points(0) {
    // Enable nested parallelism
    omp_set_nested(1);
}

struct MoveResult {
    int totalScore = 0;
    int validSims = 0;
    bool validMove = false;
};

int MCTSRandom::randomToEnd(Game2048& gameCopy) {
    int score = 0;
    std::mt19937 gen(std::random_device{}());  // Local RNG per thread
    std::uniform_int_distribution<> dis(0, 3);
    
    auto result = gameCopy.move(dis(gen));
    while (!result.gameOver) {
        score += result.reward;
        result = gameCopy.move(dis(gen));
    }
    
    return score;
}

bool MCTSRandom::makeMove() {
    const int num_boards = game.getBoards().size();
    std::vector<std::vector<MoveResult>> allResults(num_boards, std::vector<MoveResult>(4));
    
    // Parallelize across both boards and moves
    #pragma omp parallel for collapse(2)
    for (int board = 0; board < num_boards; board++) {
        for (int move = 0; move < 4; move++) {
            Game2048 testGame(game);  // Copy full game state
            auto result = testGame.moveWithoutSpawn(move);
            
            if (!result.changed) {
                allResults[board][move].validMove = false;
                continue;
            }
            
            allResults[board][move].validMove = true;
            Game2048 baseState(testGame);  // Save state after move
            
            // Run simulations for this board+move combination
            for (int sim = 0; sim < simulations; sim++) {
                Game2048 simState(baseState);
                int score = randomToEnd(simState);
                if (score > 0) {
                    allResults[board][move].totalScore += score;
                    allResults[board][move].validSims++;
                }
            }
        }
    }

    // Find best move (aggregating across all boards)
    float bestScore = -1;
    int bestMove = -1;
    
    for (int move = 0; move < 4; move++) {
        float moveScore = 0;
        int totalValid = 0;
        
        for (int board = 0; board < num_boards; board++) {
            if (allResults[board][move].validMove && allResults[board][move].validSims > 0) {
                moveScore += static_cast<float>(allResults[board][move].totalScore) / 
                            allResults[board][move].validSims;
                totalValid++;
            }
        }
        
        if (totalValid > 0) {
            moveScore /= totalValid;  // Average across boards
            if (moveScore > bestScore) {
                bestScore = moveScore;
                bestMove = move;
            }
        }
    }
    
    // Make the actual move
    auto result = game.move(bestMove);
    points += result.reward;
    
    if (result.gameOver) {
        std::cout << "Game over! Total points: " << points << std::endl;
        std::cout << "Final board state:" << std::endl;
        for (const auto& board : game.getBoards()) {
            for (int i = 0; i < 16; i++) {
                std::cout << board[i] << " ";
                if ((i + 1) % 4 == 0) std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    return result.gameOver;
}