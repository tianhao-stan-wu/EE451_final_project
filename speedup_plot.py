import matplotlib.pyplot as plt

# Data from the table
data = {
    "SEQ300": [21.2566, 14.0094, 10.498, 8.26098, 6.177, 6.2998],
    "SEQ150": [7.46859, 5.34728, 4.03481, 3.23579, 2.56588, 2.65799],
    "SEQ50": [1.28948, 1.02094, 0.77949, 0.719376, 0.651042, 0.766758],
    "SEQ20": [0.52152, 0.438433, 0.375115, 0.354693, 0.416306, 0.532877]
}

# Compute the best speedup for each SEQ
speedups = {seq: max(data[seq][0] / time for time in data[seq]) for seq in data}

# Prepare data for plotting
sequences = list(speedups.keys())
best_speedups = list(speedups.values())

# Plotting
plt.figure(figsize=(8, 6))
plt.bar(sequences, best_speedups, color='orange')
plt.xlabel("Sequences", fontsize=12)
plt.ylabel("Best Speedup", fontsize=12)
plt.title("Best Speedup for Each Sequence", fontsize=14)
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.show()
