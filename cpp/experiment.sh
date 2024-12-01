#!/bin/bash

# Number of experiments to run
N=100

# Output file
OUTPUT_FILE="experiment_results_merge.csv"

# Create CSV header
echo "experiment,max_tile,score" > $OUTPUT_FILE

# Run N experiments
for i in $(seq 1 $N); do
    echo "Running experiment $i/$N"
    
    # Run the game and capture all output
    game_output=$(OMP_NUM_THREADS=32 ./game2048 1 100)
    
    # Extract score from the "Total points:" line
    score=$(echo "$game_output" | grep "Game over! Total points:" | awk '{print $5}')
    
    # Extract final board state and find max tile
    # Get the 4 lines after "Final board state:"
    result=$(echo "$game_output" | grep -A4 "Final board state:" | tail -n 4 | tr -s ' ' | sed 's/^ //' | awk '{print $1, $2, $3, $4}')
    max_tile=$(echo "$result" | tr ' ' '\n' | sort -nr | head -n1)
    
    # Append to CSV
    echo "$i,$max_tile,$score" >> $OUTPUT_FILE
    
    echo "Experiment $i completed with max tile: $max_tile, score: $score"
done

echo "All experiments completed. Results saved in $OUTPUT_FILE"