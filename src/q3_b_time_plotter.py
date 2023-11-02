import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
data = pd.read_csv('stats/input/q3_b_time.csv')

# Extract RAID numbers and corresponding times
raid = data['RAID']
time = data['Time']

# Plot the data
plt.figure(figsize=(8, 6))
plt.plot(raid, time, marker='o', color='blue', linestyle='-', linewidth=2)

plt.xlabel('RAID')
plt.ylabel('Time(ms)')
plt.title('RAID Total Time Performance for Random Read Workloads')
plt.xticks(raid)
plt.savefig('stats/output/q3_b_time.png')

