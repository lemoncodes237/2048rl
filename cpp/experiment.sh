#!/bin/bash

# Default values
N=100
PARALLEL_RUNS=32
MCTS_TYPE="explorescore"
SIMULATIONS=100
DATA_DIR="data"
RESULTS_DIR="results"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

# Define which MCTS types use OpenMP
declare -A USES_OPENMP
USES_OPENMP=([random]=1 [merge]=1 [score]=1)

# Create directories
mkdir -p "$DATA_DIR"
mkdir -p "$RESULTS_DIR"
mkdir -p "$DATA_DIR/logs"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -n|--num-experiments)
            N="$2"
            shift 2
            ;;
        -p|--parallel)
            PARALLEL_RUNS="$2"
            shift 2
            ;;
        -m|--mcts-type)
            MCTS_TYPE="$2"
            shift 2
            ;;
        -s|--simulations)
            SIMULATIONS="$2"
            shift 2
            ;;
        *)
            echo "Unknown parameter: $1"
            exit 1
            ;;
    esac
done

# Define output files - include simulation count in filename
BASE_NAME="${MCTS_TYPE}_sims${SIMULATIONS}_${TIMESTAMP}"
OUTPUT_FILE="$DATA_DIR/${BASE_NAME}.csv"
TEMP_DIR="$DATA_DIR/temp_${TIMESTAMP}"
mkdir -p "$TEMP_DIR"

echo "Configuration:"
echo "- Number of experiments: $N"
echo "- MCTS simulations: $SIMULATIONS"
echo "- Parallel units: $PARALLEL_RUNS"
echo "- MCTS type: $MCTS_TYPE"

# Compile with specified MCTS type
echo -e "\nCompiling with MCTS_TYPE=$MCTS_TYPE..."
make clean
make MCTS_TYPE=$MCTS_TYPE || { echo "Compilation failed"; exit 1; }

# Create CSV header
echo "experiment,max_tile,score,runtime_ms" > "$OUTPUT_FILE"

if [[ ${USES_OPENMP[$MCTS_TYPE]} ]]; then
    echo "Using OpenMP parallelization (${PARALLEL_RUNS} threads)..."
    
    # Run experiments sequentially but with OpenMP threading
    for ((i = 1; i <= N; i++)); do
        echo "Running experiment $i/$N"
        
        start_time=$(date +%s%N)
        # Use OpenMP threads for internal parallelization, pass simulation count
        game_output=$(OMP_NUM_THREADS=$PARALLEL_RUNS ./game2048 1 $SIMULATIONS 2>&1)
        end_time=$(date +%s%N)
        duration_ms=$(( (end_time - start_time) / 1000000 ))
        
        # Process output
        score=$(echo "$game_output" | grep "Game over! Total points:" | awk '{print $5}')
        board_state=$(echo "$game_output" | grep -A4 "Final board state:" | tail -n 4 | tr -s ' ' | sed 's/^ //' | awk '{print $1, $2, $3, $4}')
        max_tile=$(echo "$board_state" | tr ' ' '\n' | sort -nr | head -n1)
        
        # Save result
        echo "$i,$max_tile,$score,$duration_ms" >> "$OUTPUT_FILE"
        
        # Save log
        echo -e "\nExperiment $i output:\n$game_output\n----------------------------------------" >> "$DATA_DIR/logs/${BASE_NAME}.log"
    done
else
    echo "Using process-level parallelization ($PARALLEL_RUNS parallel experiments)..."
    
    # Function to run a single experiment
    run_single_experiment() {
        local exp_num=$1
        local result_file="$TEMP_DIR/result_${exp_num}.csv"
        local log_file="$TEMP_DIR/log_${exp_num}.txt"
        
        start_time=$(date +%s%N)
        # Pass simulation count to game2048
        ./game2048 1 $SIMULATIONS > "$log_file" 2>&1
        end_time=$(date +%s%N)
        duration_ms=$(( (end_time - start_time) / 1000000 ))
        
        score=$(grep "Game over! Total points:" "$log_file" | awk '{print $5}')
        board_state=$(grep -A4 "Final board state:" "$log_file" | tail -n 4 | tr -s ' ' | sed 's/^ //' | awk '{print $1, $2, $3, $4}')
        max_tile=$(echo "$board_state" | tr ' ' '\n' | sort -nr | head -n1)
        
        echo "$exp_num,$max_tile,$score,$duration_ms" > "$result_file"
        echo "Completed experiment $exp_num"
    }
    
    export SIMULATIONS  # Make available to subprocesses
    
    # Run experiments in batches
    completed=0
    for ((i = 1; i <= N; i += PARALLEL_RUNS)); do
        batch_size=$((PARALLEL_RUNS))
        if ((i + batch_size > N)); then
            batch_size=$((N - i + 1))
        fi
        
        # Launch batch of experiments
        for ((j = 0; j < batch_size; j++)); do
            exp_id=$((i + j))
            run_single_experiment $exp_id &
        done
        
        # Wait for batch to complete
        wait
        
        # Update completion count
        completed=$((completed + batch_size))
        echo "Completed $completed/$N experiments"
    done
    
    # Combine results
    cat "$OUTPUT_FILE" > "$DATA_DIR/temp_final.csv"  # Include header
    cat "$TEMP_DIR"/result_*.csv | sort -n -t',' -k1 >> "$DATA_DIR/temp_final.csv"
    mv "$DATA_DIR/temp_final.csv" "$OUTPUT_FILE"
    
    # Combine logs
    cat "$TEMP_DIR"/log_*.txt > "$DATA_DIR/logs/${BASE_NAME}.log"
fi

# Clean up
rm -rf "$TEMP_DIR"

# Run plotting script
echo "Running analysis..."
python plotting/plot_results.py "$OUTPUT_FILE"

echo "Pipeline completed. Files saved in:"
echo "- Raw data: $OUTPUT_FILE"
echo "- Logs: $DATA_DIR/logs/${BASE_NAME}.log"
echo "- Plots: $RESULTS_DIR/${BASE_NAME}_*.png"