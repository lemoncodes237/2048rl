#!/bin/bash
#SBATCH --job-name=mcts_exp          # Job name
#SBATCH --partition=test             # Specify test partition
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # Run a single task		
#SBATCH --cpus-per-task=32          # Number of CPU cores per task
#SBATCH --mem=32gb                   # Job memory request
#SBATCH --time=04:00:00             # Time limit hrs:min:sec
#SBATCH --output=mcts_%j.log        # Standard output and error log
#SBATCH --mail-type=END,FAIL        # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=your.email@domain.com

# Set the number of OpenMP threads
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

# Create timestamp for this run
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
DATA_DIR="data"
MCTS_TYPE="explorescore"

# Create directories if they don't exist
mkdir -p "$DATA_DIR"
mkdir -p "$DATA_DIR/logs"
mkdir -p "results"

# Define C values for grid search
C_VALUES=(1000 1100 1200)

# Run experiments for each C value
for C in "${C_VALUES[@]}"; do
    echo "Running experiments with C=$C"
    
    # Run the experiment script with current C value
    ./experiment.sh \
        --num-experiments 100 \
        --parallel 32 \
        --mcts-type ${MCTS_TYPE} \
        --simulations 250 \
        --c-value $C

    # The output file will include C value
    OUTPUT_FILE="${DATA_DIR}/${MCTS_TYPE}_sims100_C${C}_${TIMESTAMP}.csv"
    
    # Run the plotting script with the CSV file path
    python plotting/plot_results.py "${OUTPUT_FILE}"
done

# Combine all results into a single CSV for comparison
echo "Combining results and creating comparison plots..."
python plotting/compare_results.py "$DATA_DIR" "${MCTS_TYPE}_sims100_C*_${TIMESTAMP}.csv"