import matplotlib.pyplot as plt

# Data from the table
threads = [1, 2, 4, 8, 16, 32]
seq300 = [21.2566, 14.0094, 10.498, 8.26098, 6.177, 6.2998]
seq150 = [7.46859, 5.34728, 4.03481, 3.23579, 2.56588, 2.65799]
seq50 = [1.28948, 1.02094, 0.77949, 0.719376, 0.651042, 0.766758]
seq20 = [0.52152, 0.438433, 0.375115, 0.354693, 0.416306, 0.532877]

# Plotting the data
plt.figure(figsize=(10, 6))
plt.plot(threads, seq300, marker='o', label='SEQ300')
plt.plot(threads, seq150, marker='o', label='SEQ150')
plt.plot(threads, seq50, marker='o', label='SEQ50')
plt.plot(threads, seq20, marker='o', label='SEQ20')

# Adding titles and labels
plt.title('Execution Time vs Number of Threads', fontsize=16)
plt.xlabel('Number of Threads', fontsize=14)
plt.ylabel('Execution Time (s)', fontsize=14)
plt.xscale('log', base=2)  # Logarithmic scale for x-axis (base 2)
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.xticks(threads, labels=[str(t) for t in threads])  # Ensure all thread counts appear on x-axis
plt.legend(fontsize=12)

# Save and show the plot
plt.tight_layout()
plt.savefig("execution_time_vs_threads.png")
plt.show()
