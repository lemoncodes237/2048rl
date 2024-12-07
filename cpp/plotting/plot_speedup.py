import matplotlib.pyplot as plt
import numpy as np

# Data
threads = np.array([1, 2, 4, 8, 16, 32])
time = np.array([10.954, 4.733, 2.065, 1.862, 0.632, 0.415])
moves_per_sec = np.array([128.989, 247.601, 458.016, 1010.148, 1490.921, 2138.509])
speedup = time[0] / time  # Calculate speedup relative to single thread

# Create two subplots
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 12))

# First subplot: Execution Time and Speedup
color1, color2 = '#1f77b4', '#2ca02c'
line1 = ax1.plot(threads, time, 'o-', color=color1, linewidth=2, label='Execution Time')
ax1.set_xlabel('Number of Threads')
ax1.set_ylabel('Execution Time (seconds)', color=color1)
ax1.tick_params(axis='y', labelcolor=color1)

# Add second y-axis for speedup
ax1_twin = ax1.twinx()
line2 = ax1_twin.plot(threads, speedup, 's-', color=color2, linewidth=2, label='Speedup')
ax1_twin.set_ylabel('Speedup (x)', color=color2)
ax1_twin.tick_params(axis='y', labelcolor=color2)

# Add legend
lines1, labels1 = ax1.get_legend_handles_labels()
lines2, labels2 = ax1_twin.get_legend_handles_labels()
ax1.legend(lines1 + lines2, labels1 + labels2, loc='upper right')

ax1.set_title('Execution Time and Speedup vs Number of Threads')
ax1.grid(True)
ax1.set_xscale('log', base=2)

# Second subplot: Moves per Second
ax2.plot(threads, moves_per_sec, 'o-', color='#ff7f0e', linewidth=2, label='Actual Performance')
ax2.set_xlabel('Number of Threads')
ax2.set_ylabel('Moves per Second')
ax2.set_title('Performance Scaling: Moves per Second vs Number of Threads')
ax2.grid(True)
ax2.set_xscale('log', base=2)

# Add ideal scaling line for comparison
ideal_scaling = moves_per_sec[0] * threads / threads[0]
ax2.plot(threads, ideal_scaling, '--', color='gray', label='Ideal Linear Scaling')
ax2.legend()

# Adjust layout and display
plt.tight_layout()
plt.savefig('performance_plots.png', dpi=300, bbox_inches='tight')
plt.show()