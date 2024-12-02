// mcts_random.cpp
#include "mcts_pUCT.h"
#include <random>
#include <algorithm>
#include <vector>
#include <iostream>
#include <omp.h>
#include <iomanip>
#include <cassert>
MCTSpUCT::MCTSpUCT(int n, int simulations) 
    : game(n), currGame(nullptr), simulations(simulations), points(0), acquired(0) {
    // Enable nested parallelism
    omp_set_nested(1);
}

struct MoveResult {
    int totalScore = 0;
    int validSims = 0;
    bool validMove = false;
};

pUCTNode::pUCTNode(unsigned long state, bool chance, int a)
    : value(0), visits(0), chance(chance), state(state)  {
        if(chance)  {
            action = a;
        } else  {
            action = -1;
        }
    }

void pUCTNode::incrementVisits()  { visits++; }
void pUCTNode::increaseValue(double v) { value += v; }

int MCTSpUCT::randomToEnd() {
    // Create a fresh copy for this simulation
    Game2048 gameCopy(*currGame);
    
    // First apply the move we're testing
    int score = 0;
    Game2048::MoveResult result;
    result.gameOver = false;

    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    // Then do moves that maximize the merges until game over
    while (!result.gameOver) {
        // Test each possible move
        std::vector<double> scores(4);
        double sum = 0;
        for (int move = 0; move < 4; move++) {
            // Test if move is valid using a temporary copy
            Game2048 testGame(gameCopy);
            auto moveResult = testGame.moveWithoutSpawn(move);
            
            if (!moveResult.changed) {
                continue;
            }
            
            // The addition by 1 ensures that only moves that change the board are selected
            scores[move] = moveResult.reward + 1;
            sum += scores[move];
        }

        for (int move = 0; move < 4; move++)  {
            scores[move] /= sum;
        }

        double val = dis(gen);
        int nextMove = 0;
        double cp = 0.0;
        // Choose move proportional to score
        for (int move = 0; move < 4; move++)  {
            cp += scores[move];
            if(val < cp)  {
                nextMove = move;
                break;
            }
        }

        result = gameCopy.move(nextMove);
        score += result.reward;
    }
    
    return score;
}

/*int MCTSpUCT::randomToEnd() {
    // Create a fresh copy for this simulation
    Game2048 gameCopy(*currGame);
    
    // Then do random moves until game over
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis(0, 3);

    // First apply the move we're testing
    auto result = gameCopy.move(dis(gen));
    int score = result.reward;

    int moves = 0;
    
    while (!result.gameOver) {
        result = gameCopy.move(dis(gen));
        score += result.reward;
        ++moves;

        if(moves > 10000)  {
            std::cout << score << "\n";
        }
    }

    //for(auto board : gameCopy.getBoards())  {
    //    for(int i = 0; i < 16; i++)  {
    //        score += board[i];
    //    }
    //}
    
    return score;
}*/

unsigned long MCTSpUCT::getBoardNum()  {
    unsigned long val = 0;
    // Note: SHOULD ONLY USE ONE BOARD
    for(auto board : currGame->getBoards())  {
        for(int i = 0; i < 16; i++)  {
            val <<= 4;

            unsigned long log = 0;
            unsigned long tile = board[i];

            while(tile > 0)  {
                ++log;
                tile /= 2;
            }
            val += log;
        }
    }

    return val;
}

int MCTSpUCT::selectAction(pUCTNode* node)  {
    if(node->children.size() != 4)  {
        //std::cout << "Not enough children\n";
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dis(0, 3 - node->children.size());
        int take = dis(gen);

        std::vector<bool> exist(4);
        for(auto child : node->children)  {
            exist[child->action] = true;
        }

        int index = 0;
        for(int i = 0; i < 4; i++)  {
            if(exist[i]) continue;

            if(take == index)  {
                return i;
            }
            ++index;
        }

        return -1;
    } else  {
        double bestUCB = -1;
        int a = -1;

        for(auto child : node->children)  {
            double ucb = child->value / child->visits + C * sqrt(log(node->visits) / child->visits);
            //std::cout << "UCB " << ucb << "\n";
            if(ucb > bestUCB)  {
                bestUCB = ucb;
                a = child->action;
            }
        }

        return a;
    }
}

double MCTSpUCT::sample(pUCTNode* node)  {
    // Create a fresh copy for this simulation
    double before = node->value;

    if(node->chance)  {
        int acq = acquired;
        auto children = node->children;
        unsigned long state = getBoardNum();
        pUCTNode* curr = nullptr;

        for(auto child : children)  {
            if(child->state == state)  {
                //std::cout << "Already had child with state " << state << "\n";
                curr = child;
                break;
            }
        }

        if(!curr)  {
            curr = new pUCTNode(state, false, -1);
            node->children.push_back(curr);
        }

        node->value += sample(curr) + acq;
    } else if(node->visits == 0.0)  {
        node->value = randomToEnd();
    } else  {
        int a = selectAction(node);
        
        pUCTNode* curr = nullptr;
        auto children = node->children;

        for(auto child : children)  {
            if(child->action == a)  {
                curr = child;
                break;
            }
        }
        if(!curr)  {
            curr = new pUCTNode(0, true, a);
            node->children.push_back(curr);
        }
        auto result = currGame->move(a);

        acquired = result.reward;

        if(!result.changed)  {
            curr->visits = 100000;
        }

        if(result.gameOver)  {
            curr->visits++;
        } else  {
            node->value += sample(curr);
        }
    }

    node->visits += 1;

    //std::cout << "Returning " << node->value-before << "\n";

    return node->value - before;
}

bool MCTSpUCT::makeMove() {
    std::vector<float> rewards(4);
    
    // Test each possible move
    //for (int move = 0; move < 4; move++) {
        // Test if move is valid using a temporary copy

        currGame = &game;
        pUCTNode node(getBoardNum(), false, -1);

        for(int sim = 0; sim < simulations; sim++)  {
            Game2048 copyGame(game);
            currGame = &copyGame;

            //std::cout << "Calling sample\n";
            sample(&node);
        }

        //#pragma omp parallel for reduction(+:score_sum)
        //for (int sim = 0; sim < simulations; sim++) {
        //    score_sum += randomToEnd(move);
        //}
    //}

    std::vector<double> valuevalue(4);
    std::vector<double> visitsvisits(4);
    for(int move = 0; move < 4; move++)  {
        Game2048 testGame(game);
        auto moveResult = testGame.moveWithoutSpawn(move);
        
        if (!moveResult.changed) {
            rewards[move] = -1;
            continue;
        }

        for(auto child : node.children)  {
            if(child->action == move)  {
                rewards[move] = (float) child->value / child->visits;
                valuevalue[move] = child->value;
                visitsvisits[move] = child->visits;
                break;
            }
        }
    }
    
    // Find best move
    int bestMove = 0;
    float bestScore = rewards[0];
    std::cout << "Board state\n";
    for (const auto& board : game.getBoards()) {
        for (int i = 0; i < 16; i++) {
            std::cout << std::setw(5) << board[i] << " ";
            if ((i + 1) % 4 == 0) std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << "Rewards:\n" << bestScore << " " << valuevalue[0] << " " << visitsvisits[0] << std::endl;
    for (int move = 1; move < 4; move++) {
        std::cout << rewards[move] << " " << valuevalue[move] << " " << visitsvisits[move] << std::endl;
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