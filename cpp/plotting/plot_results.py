#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from pathlib import Path
import argparse

def setup_plotting_style():
    """Configure the plotting style for consistent visualizations."""
    plt.rcParams['figure.figsize'] = [12, 6]
    plt.rcParams['figure.dpi'] = 300

def create_tile_distribution_plot(df, ax, title=None):
    """Create a bar plot showing the distribution of maximum tiles achieved."""
    unique_tiles, counts = np.unique(df['max_tile'], return_counts=True)
    tile_freq = dict(zip(unique_tiles, counts))
    sorted_tiles = sorted(tile_freq.items())
    tiles, frequencies = zip(*sorted_tiles)
    
    bars = ax.bar(range(len(tiles)), frequencies)
    ax.set_xticks(range(len(tiles)))
    ax.set_xticklabels([str(t) for t in tiles], rotation=45)
    
    # Add value labels on top of each bar
    for bar, freq in zip(bars, frequencies):
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height,
                f'{freq}\n({freq/len(df)*100:.1f}%)',
                ha='center', va='bottom')
    
    ax.set_title('Distribution of Maximum Tile Values' if title is None else title)
    ax.set_xlabel('Tile Value')
    ax.set_ylabel('Frequency')
    ax.grid(True, axis='y', linestyle='--', alpha=0.7)

def create_score_distribution_plot(df, ax, title=None):
    """Create a histogram showing the distribution of scores."""
    sns.histplot(data=df, x='score', kde=True, ax=ax)
    ax.axvline(x=df['score'].mean(), color='r', linestyle='--',
               label=f'Mean: {int(df["score"].mean()):,}')
    ax.axvline(x=df['score'].median(), color='g', linestyle='--',
               label=f'Median: {int(df["score"].median()):,}')
    
    ax.set_title('Distribution of Scores' if title is None else title)
    ax.set_xlabel('Score')
    ax.set_ylabel('Frequency')
    ax.legend()

def print_statistics(df):
    """Print summary statistics for the experiment results."""
    print("\nExperiment Statistics:")
    print(f"Number of games: {len(df)}")
    
    print("\nMax Tile Statistics:")
    print(f"Highest tile achieved: {df['max_tile'].max()}")
    unique_tiles, counts = np.unique(df['max_tile'], return_counts=True)
    print("\nTile frequency distribution:")
    for tile, count in zip(unique_tiles, counts):
        print(f"Tile {tile}: {count} times ({count/len(df)*100:.1f}%)")
    
    print("\nScore Statistics:")
    print(f"Highest score: {int(df['score'].max()):,}")
    print(f"Average score: {int(df['score'].mean()):,}")
    print(f"Median score: {int(df['score'].median()):,}")
    print(f"Standard deviation: {int(df['score'].std()):,}")
    
    if 'runtime_ms' in df.columns:
        print("\nRuntime Statistics:")
        print(f"Average runtime: {df['runtime_ms'].mean():.2f}ms")
        print(f"Median runtime: {df['runtime_ms'].median():.2f}ms")
        print(f"Min runtime: {df['runtime_ms'].min():.2f}ms")
        print(f"Max runtime: {df['runtime_ms'].max():.2f}ms")

def analyze_and_plot(csv_path):
    """Main function to analyze results and create visualizations."""
    # Read the data
    df = pd.read_csv(csv_path)
    
    # Setup plotting style
    setup_plotting_style()
    
    # Create figure with two subplots side by side
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    # Create plots
    create_tile_distribution_plot(df, ax1)
    create_score_distribution_plot(df, ax2)
    
    # Add overall title
    mcts_type = Path(csv_path).stem.split('_')[0]  # Extract MCTS type from filename
    plt.suptitle(f'2048 MCTS Results (Type: {mcts_type}, N={len(df)})',
                 y=1.05, fontsize=14)
    
    # Adjust layout and save
    plt.tight_layout()
    
    # Save to results directory
    output_path = Path('results') / f"{Path(csv_path).stem}_analysis.png"
    plt.savefig(output_path, bbox_inches='tight', dpi=300)
    plt.close()
    
    # Print statistics
    print_statistics(df)
    
    return output_path

def main():
    parser = argparse.ArgumentParser(description='Analyze and plot 2048 MCTS results')
    parser.add_argument('csv_file', type=str, help='Path to the CSV file with results')
    args = parser.parse_args()
    
    try:
        output_path = analyze_and_plot(args.csv_file)
        print(f"\nPlot saved to: {output_path}")
    except Exception as e:
        print(f"Error processing results: {e}")
        raise

if __name__ == "__main__":
    main()