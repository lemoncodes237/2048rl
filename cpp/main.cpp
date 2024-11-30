#include "mcts_random.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std::chrono;

struct GameStats {
    int total_moves;
    int final_score;
    double total_time;
    double avg_time_per_move;
};

GameStats run_game(int num_boards, int num_simulations) {
    MCTSRandom mcts(num_boards, num_simulations);
    GameStats stats = {0, 0, 0.0, 0.0};
    auto start_time = high_resolution_clock::now();
    
    while (!mcts.makeMove()) {
        stats.total_moves++;
    }
    
    auto end_time = high_resolution_clock::now();
    stats.total_time = duration<double>(end_time - start_time).count();
    stats.avg_time_per_move = stats.total_time / stats.total_moves;
    stats.final_score = mcts.getPoints();
    
    return stats;
}

void print_stats(const GameStats& stats) {
    std::cout << "\n=== Performance Statistics ===\n";
    std::cout << "Total moves: " << stats.total_moves << "\n";
    std::cout << "Final score: " << stats.final_score << "\n";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Total time: " << stats.total_time << " seconds\n";
    std::cout << "Average time per move: " << stats.avg_time_per_move * 1000 << " ms\n";
    std::cout << "Moves per second: " << stats.total_moves / stats.total_time << "\n";
}

int main(int argc, char* argv[]) {
    int num_boards = 8;
    int num_simulations = 200;
    
    if (argc > 1) num_boards = std::atoi(argv[1]);
    if (argc > 2) num_simulations = std::atoi(argv[2]);
    
    std::cout << "Running with " << num_boards << " boards and " 
              << num_simulations << " simulations per move\n";
    
    #ifdef _OPENMP
    std::cout << "OpenMP threads: " << omp_get_max_threads() << "\n";
    #endif
    
    auto stats = run_game(num_boards, num_simulations);
    print_stats(stats);
    
    return 0;
}