import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from pathlib import Path
import glob
import sys
import re

def setup_plotting_style():
    """Configure the plotting style for consistent visualizations."""
    plt.style.use('seaborn')
    plt.rcParams['figure.figsize'] = [15, 10]
    plt.rcParams['figure.dpi'] = 300

def extract_c_value(filename):
    """Extract C value from filename."""
    match = re.search(r'C(\d+)', filename)
    return int(match.group(1)) if match else None

def combine_and_compare_results(data_dir, pattern):
    """Combine results from multiple C values and create comparison plots."""
    # Find all matching CSV files
    files = glob.glob(str(Path(data_dir) / pattern))
    
    # Read and combine all data
    all_data = []
    for file in files:
        df = pd.read_csv(file)
        c_value = extract_c_value(file)
        df['C'] = c_value
        all_data.append(df)
    
    combined_df = pd.concat(all_data, ignore_index=True)
    
    # Create comparison plots
    setup_plotting_style()
    
    # 1. Box plot of scores for each C value
    plt.figure(figsize=(15, 8))
    sns.boxplot(data=combined_df, x='C', y='score')
    plt.title('Score Distribution by C Value')
    plt.xlabel('C Value')
    plt.ylabel('Score')
    output_path = Path('results') / f'score_distribution_by_C_{Path(files[0]).stem.split("_")[0]}.png'
    plt.savefig(output_path, bbox_inches='tight')
    plt.close()
    
    # 2. Max tile achievement rate by C value
    plt.figure(figsize=(15, 8))
    tile_counts = combined_df.groupby(['C', 'max_tile']).size().unstack(fill_value=0)
    tile_percentages = tile_counts.div(tile_counts.sum(axis=1), axis=0) * 100
    
    tile_percentages.plot(kind='bar', stacked=True)
    plt.title('Max Tile Achievement Rate by C Value')
    plt.xlabel('C Value')
    plt.ylabel('Percentage')
    plt.legend(title='Max Tile', bbox_to_anchor=(1.05, 1), loc='upper left')
    output_path = Path('results') / f'max_tile_by_C_{Path(files[0]).stem.split("_")[0]}.png'
    plt.savefig(output_path, bbox_inches='tight')
    plt.close()
    
    # Print statistics
    print("\nStatistics by C value:")
    stats = combined_df.groupby('C').agg({
        'score': ['mean', 'std', 'median', 'max'],
        'max_tile': 'max'
    }).round(2)
    print(stats)
    
    # Save combined results
    output_path = Path('results') / f'combined_results_{Path(files[0]).stem.split("_")[0]}.csv'
    combined_df.to_csv(output_path, index=False)
    print(f"\nCombined results saved to: {output_path}")

def main():
    if len(sys.argv) < 3:
        print("Usage: python compare_results.py <data_dir> <pattern>")
        sys.exit(1)
    
    data_dir = sys.argv[1]
    pattern = sys.argv[2]
    combine_and_compare_results(data_dir, pattern)

if __name__ == "__main__":
    main()