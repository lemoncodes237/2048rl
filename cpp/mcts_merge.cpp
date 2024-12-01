// mcts_random.cpp
#include "mcts_merge.h"
#include <random>
#include <algorithm>
#include <vector>
#include <iostream>
#include <omp.h>
#include <iomanip>
MCTSMerge::MCTSMerge(int n, int simulations) 
    : game(n), simulations(simulations), points(0) {
    // Enable nested parallelism
    omp_set_nested(1);
}

struct MoveResult {
    int totalScore = 0;
    int validSims = 0;
    bool validMove = false;
};

int MCTSMerge::moveToEnd(int move) {
    // Create a fresh copy for this simulation
    Game2048 gameCopy(game);
    
    // First apply the move we're testing
    auto result = gameCopy.move(move);
    int score = result.reward;

    std::mt19937 gen(std::random_device{}());
    
    // Then do moves that maximize the merges until game over
    while (!result.gameOver) {
        // Test each possible move
        std::vector<int> bestMoves(4);
        int bestMerge = -1;
        int nMoves = 0;
        for (int move = 0; move < 4; move++) {
            // Test if move is valid using a temporary copy
            Game2048 testGame(gameCopy);
            auto moveResult = testGame.moveWithoutSpawn(move);
            
            if (!moveResult.changed) {
                continue;
            }
            
            if(bestMerge < moveResult.merges)  {
                bestMoves[0] = move;
                nMoves = 1;
                bestMerge = moveResult.merges;
            } else if(bestMerge == moveResult.merges)  {
                bestMoves[nMoves] = move;
                nMoves++;
            }
        }

        std::uniform_int_distribution<> dis(0, nMoves - 1);

        result = gameCopy.move(bestMoves[dis(gen)]);
        score += result.reward;
    }
    
    return score;
}

bool MCTSMerge::makeMove() {
    std::vector<float> rewards(4);
    
    // Test each possible move
    for (int move = 0; move < 4; move++) {
        // Test if move is valid using a temporary copy
        Game2048 testGame(game);
        auto moveResult = testGame.moveWithoutSpawn(move);
        
        if (!moveResult.changed) {
            rewards[move] = -1;
            continue;
        }
        
        // Run B simulations for this move
        int score_sum = 0;
        #pragma omp parallel for reduction(+:score_sum)
        for (int sim = 0; sim < simulations; sim++) {
            score_sum += moveToEnd(move);
        }
        rewards[move] = score_sum;
    }
    
    // Find best move
    int bestMove = 0;
    float bestScore = rewards[0];
    for (int move = 1; move < 4; move++) {
        if (rewards[move] > bestScore) {
            bestScore = rewards[move];
            bestMove = move;
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
                std::cout << std::setw(5) << board[i] << " ";
                if ((i + 1) % 4 == 0) std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    return result.gameOver;
}