// mcts_random.cpp
#include "mcts_explore_score.h"
#include <random>
#include <algorithm>
#include <vector>
#include <iostream>
#include <omp.h>
#include <iomanip>
#include <cmath>
MCTSExploreScore::MCTSExploreScore(int n, int simulations) 
    : game(n), simulations(simulations), points(0) {
    // Enable nested parallelism
    omp_set_nested(1);
}

struct MoveResult {
    int totalScore = 0;
    int validSims = 0;
    bool validMove = false;
};

std::string MCTSExploreScore::getBoardString(Game2048 board)  {
    std::string s = "";

    for (const auto& ind_board : board.getBoards()) {
        for (int i = 0; i < 16; i++) {
            s = s + " " + std::to_string(ind_board[i]);
        }
    }

    return s;
}

int MCTSExploreScore::moveToEnd(std::map<std::string, double> &scores, std::map<std::string, double> &visits) {
    // Create a fresh copy for this simulation
    Game2048 gameCopy(game);
    
    // First apply the move we're testing
    Game2048::MoveResult result;
    result.gameOver = false;
    int score = 0;

    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    int iter = 0;
    std::vector<std::string> positions;
    
    while (!result.gameOver) {
        int nextMove = 0;

        if(iter < T)  {
            std::vector<int> unexploredMoves(4);
            int unexplored = 0;
            std::vector<double> ucbScores(4);

            std::string curr = getBoardString(gameCopy);
            if(visits.find(curr) == visits.end())  {
                visits[curr] = 0;
            }
            visits[curr]++;

            //std::cout << "Move calc" << std::endl;

            for(int move = 0; move < 4; move++)  {
                Game2048 testGame(gameCopy);
                auto moveResult = testGame.move(move);

                if(!moveResult.changed)  {
                    continue;
                }

                std::string s = getBoardString(testGame);
                positions.push_back(s);

                if(visits.find(s) == visits.end())  {
                    unexploredMoves[unexplored] = move;
                    unexplored++;
                } else  {
                    if(visits[curr] == 1)  {
                        ucbScores[move] = (scores[s] / visits[s]);
                    } else {
                        ucbScores[move] = (scores[s] / visits[s]) + C * sqrt(log(visits[curr]) / visits[s]);
                    }
                }
            }

            if(unexplored > 0)  {
                std::uniform_int_distribution<> dis(0, unexplored - 1);
                nextMove = unexploredMoves[dis(gen)];
            } else  {
                double high = -1;
                for(int move = 0; move < 4; move++)  {
                    if(ucbScores[move] > high)  {
                        nextMove = move;
                        high = ucbScores[move];
                    }
                }
            }

            //std::cout << "Move calc done" << std::endl;
        } else  {
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
            double cp = 0.0;
            // Choose move proportional to score
            for (int move = 0; move < 4; move++)  {
                cp += scores[move];
                if(val < cp)  {
                    nextMove = move;
                    break;
                }
            }
        }

        result = gameCopy.move(nextMove);
        score += result.reward;
        ++iter;
    }

    // Backpropagation
    for(auto s : positions)  {
        if(visits.find(s) == visits.end())  {
            scores[s] = score;
            visits[s] = 1;
        } else  {
            visits[s]++;
            scores[s] += score;
        }
    }
    
    return score;
}

bool MCTSExploreScore::makeMove() {
    std::vector<float> rewards(4);
    std::map<std::string, double> scores;
    std::map<std::string, double> visits;

    std::vector<double> ucbScores(4);

    std::string curr = getBoardString(game);
    visits[curr] = 1;
    int score_sum = 0;
    {
        //#pragma omp parallel for reduction(+:score_sum)
        for (int sim = 0; sim < simulations; sim++) {
            score_sum += moveToEnd(scores, visits);
        }
    }

    for(int move = 0; move < 4; move++)  {
        Game2048 testGame(game);
        auto moveResult = testGame.move(move);

        if(!moveResult.changed)  {
            continue;
        }

        std::string s = getBoardString(testGame);
        ucbScores[move] = (scores[s] / visits[s]) + C * sqrt(log(visits[curr]) / visits[s]);
    }
    
    
    // Find best move
    int bestMove = 0;
    float bestScore = ucbScores[0];
    for (int move = 1; move < 4; move++) {
        if (ucbScores[move] > bestScore) {
            bestScore = ucbScores[move];
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