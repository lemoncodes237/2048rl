import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# Read the CSV file
df = pd.read_csv('experiment_results.csv')

# Create a figure with two subplots side by side
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))

# Plot 1: Max Tile Distribution
unique_tiles, counts = np.unique(df['max_tile'], return_counts=True)
tile_freq = dict(zip(unique_tiles, counts))
sorted_tiles = sorted(tile_freq.items())
tiles, frequencies = zip(*sorted_tiles)

ax1.bar(range(len(tiles)), frequencies)
ax1.set_xticks(range(len(tiles)))
ax1.set_xticklabels([str(t) for t in tiles], rotation=45)

# Add value labels on top of each bar
for i, freq in enumerate(frequencies):
    ax1.text(i, freq, f'{freq}\n({freq/len(df)*100:.1f}%)', 
             ha='center', va='bottom')

ax1.set_title('Distribution of Maximum Tile Values')
ax1.set_xlabel('Tile Value')
ax1.set_ylabel('Frequency')
ax1.grid(True, axis='y', linestyle='--', alpha=0.7)

# Plot 2: Score Distribution
sns.histplot(data=df, x='score', kde=True, ax=ax2)
ax2.axvline(x=df['score'].mean(), color='r', linestyle='--', 
            label=f'Mean: {int(df["score"].mean())}')
ax2.axvline(x=df['score'].max(), color='g', linestyle='--', 
            label=f'Max: {int(df["score"].max())}')
ax2.set_title('Distribution of Scores')
ax2.set_xlabel('Score')
ax2.set_ylabel('Frequency')
ax2.legend()

plt.tight_layout()
plt.savefig('experiment_results.png')
plt.close()

# Print statistics
print("\nExperiment Statistics:")
print(f"Number of games: {len(df)}")
print("\nMax Tile Statistics:")
print(f"Highest tile achieved: {df['max_tile'].max()}")
print("\nTile frequency distribution:")
for tile, count in sorted_tiles:
    print(f"Tile {tile}: {count} times ({count/len(df)*100:.1f}%)")

print("\nScore Statistics:")
print(f"Highest score: {int(df['score'].max())}")
print(f"Average score: {int(df['score'].mean())}")
print(f"Median score: {int(df['score'].median())}")
print(f"Standard deviation: {int(df['score'].std())}")