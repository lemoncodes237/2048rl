// env2048.cpp
#include "env2048.h"
#include <algorithm>
#include <ctime>

Game2048::Game2048(int numBoards) : 
    numBoards(numBoards),
    boards(numBoards, std::vector<int>(16, 0)),
    rng(std::time(nullptr)) {
    for (auto& board : boards) {
        genRandom(board);
        genRandom(board);
    }
}

Game2048::Game2048(const Game2048& other) : 
    numBoards(other.numBoards),
    boards(other.boards),
    rng(std::time(nullptr)) {}

Game2048::Game2048(std::vector<int>& board):
    numBoards(1),
    boards(1, board),
    rng(std::time(nullptr)) {}

void Game2048::genRandom(std::vector<int>& board) {
    std::vector<int> emptySpots;
    for (int i = 0; i < 16; i++) {
        if (board[i] == 0) emptySpots.push_back(i);
    }
    
    if (emptySpots.empty()) return;
    
    std::uniform_int_distribution<> spotDist(0, emptySpots.size() - 1);
    std::uniform_real_distribution<> valueDist(0, 1);
    
    int spot = emptySpots[spotDist(rng)];
    board[spot] = (valueDist(rng) < 0.9) ? 2 : 4;
}

std::vector<int> Game2048::moveLine(const std::vector<int>& line, bool& moved, int& score) const {
    // The final element is the number of merges
    std::vector<int> result(5, 0);
    moved = false;
    score = 0;
    int writePos = 3;
    for (int i = 3; i >= 0; i--) {
        if (line[i] == 0) continue;
        
        if (result[writePos] == 0) {
            result[writePos] = line[i];
            if (i != writePos) moved = true;
        }
        else if (result[writePos] == line[i]) {
            result[writePos] *= 2;
            score += result[writePos];
            // Increment number of merges
            result[4]++;
            writePos--;
            moved = true;
        }
        else {
            writePos--;
            result[writePos] = line[i];
            if (i != writePos) moved = true;
        }
    }
    
    return result;
}

Game2048::MoveResult Game2048::moveWithoutSpawn(int direction) {
    MoveResult result;
    result.gameOver = false;
    result.changed = false;
    result.reward = 0;
    result.merges = 0;

    for (int b = 0; b < numBoards; b++) {
        bool boardChanged = false;
        auto& board = boards[b];
        
        for (int i = 0; i < 4; i++) {
            std::vector<int> line(4);
            bool lineMoved;
            int lineScore;
            
            // Extract line based on direction
            switch (direction) {
                case 0: // Up
                    for (int j = 0; j < 4; j++) line[j] = board[(3-j)*4 + i];
                    break;
                case 1: // Down
                    for (int j = 0; j < 4; j++) line[j] = board[j*4 + i];
                    break;
                case 2: // Right
                    for (int j = 0; j < 4; j++) line[j] = board[i*4 + j];
                    break;
                case 3: // Left
                    for (int j = 0; j < 4; j++) line[j] = board[i*4 + (3-j)];
                    break;
            }
            
            auto newLine = moveLine(line, lineMoved, lineScore);
            boardChanged |= lineMoved;
            result.reward += lineScore;
            result.merges += newLine[4];
            
            // Write back
            switch (direction) {
                case 0: // Up
                    for (int j = 0; j < 4; j++) board[(3-j)*4 + i] = newLine[j];
                    break;
                case 1: // Down
                    for (int j = 0; j < 4; j++) board[j*4 + i] = newLine[j];
                    break;
                case 2: // Right
                    for (int j = 0; j < 4; j++) board[i*4 + j] = newLine[j];
                    break;
                case 3: // Left
                    for (int j = 0; j < 4; j++) board[i*4 + (3-j)] = newLine[j];
                    break;
            }
        }
        
        result.changed |= boardChanged;
    }
    
    return result;
}

Game2048::MoveResult Game2048::move(int direction) {
    auto result = moveWithoutSpawn(direction);
    
    if (result.changed) {
        for (auto& board : boards) {
            genRandom(board);
            if (isGameOver(board)) {
                result.gameOver = true;
                break;
            }
        }
    }
    
    return result;
}

bool Game2048::isGameOver(const std::vector<int>& board) const {
    // Check for empty cells
    for (int i = 0; i < 16; i++) {
        if (board[i] == 0) return false;
    }
    
    // Check for possible merges
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int idx = i*4 + j;
            if (j < 3 && board[idx] == board[idx + 1]) return false;
            if (i < 3 && board[idx] == board[idx + 4]) return false;
        }
    }
    
    return true;
}