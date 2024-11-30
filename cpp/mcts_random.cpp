// mcts_random.cpp
#include "mcts_random.h"
#include <random>
#include <algorithm>
#include <vector>
#include <iostream>

MCTSRandom::MCTSRandom(int n, int simulations) 
    : game(n), simulations(simulations), points(0) {}

int MCTSRandom::randomToEnd(int firstMove) {
    Game2048 gameCopy(game);  // Create a copy for simulation
    int score = 0;
    
    auto result = gameCopy.move(firstMove);
    if (!result.changed) return -1;  // Invalid move
    
    score += result.reward;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 3);
    
    while (!result.gameOver) {
        result = gameCopy.move(dis(gen));
        score += result.reward;
    }
    
    return score;
}

bool MCTSRandom::makeMove() {
    std::vector<int> rewards(4, -1);
    
    // Simulate moves in parallel
    #pragma omp parallel for schedule(dynamic)
    for (int move = 0; move < 4; move++) {
        Game2048 testGame(game);
        auto result = testGame.moveWithoutSpawn(move);
        
        if (!result.changed) continue;
        
        int scoreSum = 0;
        for (int sim = 0; sim < simulations; sim++) {
            int score = randomToEnd(move);
            if (score > 0) scoreSum += score;
        }
        rewards[move] = scoreSum;
    }
    
    // Find best move
    int bestMove = std::max_element(rewards.begin(), rewards.end()) - rewards.begin();
    
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