#include <iostream>
#include <chrono>
#include <iomanip>
#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef USE_RANDOM_RANDOM
#include "random_random/mcts_random_random.h"
typedef MCTSRandomRandom MCTSImpl;
#endif
#ifdef USE_MERGE
#include "merge/mcts_merge.h"
typedef MCTSMerge MCTSImpl;
#endif
#ifdef USE_RANDOM
#include "random/mcts_random.h"
typedef MCTSRandom MCTSImpl;
#endif
#ifdef USE_SCORE
#include "score/mcts_score.h"
typedef MCTSScore MCTSImpl;
#endif
#ifdef USE_PUCT_SINGLE
#include "pUCT/mcts_pUCT.h"
typedef MCTSpUCT MCTSImpl;
#endif
#ifdef USE_PUCT_MULTIPLE
#include "pUCT_multiple/mcts_pUCT.h"
typedef MCTSpUCT MCTSImpl;
#endif
#ifdef USE_PUCT_MIN_MULTIPLE
#include "pUCT_min_multiple/mcts_pUCT.h"
typedef MCTSpUCT MCTSImpl;
#endif

using namespace std::chrono;

struct GameStats {
    int total_moves;
    int final_score;
    double total_time;
    double avg_time_per_move;
};

GameStats run_game(int num_boards, int num_simulations, double c_param) {
    MCTSImpl mcts(num_boards, num_simulations, c_param);
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
    int num_boards = 1;
    int num_simulations = 250;
    double c_param = 800.0;  // Default C value

    if (argc > 1) num_boards = std::atoi(argv[1]);
    if (argc > 2) num_simulations = std::atoi(argv[2]);
    if (argc > 3) c_param = std::atof(argv[3]);

    std::cout << "Running with " << num_boards << " boards and " 
              << num_simulations << " simulations per move\n";
    
    #ifdef USE_RANDOM_RANDOM
    std::cout << "Using Random-Random MC\n";
    #endif
    #ifdef USE_MERGE
    std::cout << "Using Merge MC\n";
    #endif
    #ifdef USE_RANDOM
    std::cout << "Using Standard MC\n";
    #endif
    #ifdef USE_SCORE
    std::cout << "Using Score MC\n";
    #endif
    #ifdef USE_PUCT_SINGLE
    std::cout << "Using Single pUCT MCTS\n";
    #endif
    #ifdef USE_PUCT_MULTIPLE
    std::cout << "Using Multiple pUCT MCTS\n";
    #endif
    #ifdef USE_PUCT_MIN_MULTIPLE
    std::cout << "Using Min-Multiple pUCT MCTS\n";
    #endif
    
    #ifdef _OPENMP
    std::cout << "OpenMP threads: " << omp_get_max_threads() << "\n";
    #endif
    
    auto stats = run_game(num_boards, num_simulations, c_param);
    print_stats(stats);
    return 0;
}