import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read the CSV file
data = pd.read_csv('stats/input/q3_b_io.csv', index_col=0)

x_values = data.columns.astype(int)
# Plot the data
plt.figure(figsize=(8, 6))

# Iterate through each row and plot
for idx, row in data.iterrows():
    noise = np.random.uniform(-0.2, 0.2, len(row))  # Generating random noise
    plt.plot(np.arange(len(row)) + noise, row, label=f'RAID {idx}')

plt.xlabel('Disk')
plt.ylabel('Number of I/Os')
plt.title('RAID I/O Performance for Random Read Workloads')
plt.legend()
plt.xticks(x_values)
plt.savefig('stats/output/q3_b_io.png')
