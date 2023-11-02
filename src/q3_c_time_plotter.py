import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
data = pd.read_csv('stats/input/q3_c_time.csv')

# Extract RAID numbers, read, and write times
raid = data['RAID']
read_times = data['Read']
write_times = data['Write']

# Plot the data
plt.figure(figsize=(8, 6))
plt.plot(raid, read_times, marker='o', linestyle='-', color='blue', label='Read')
plt.plot(raid, write_times, marker='o', linestyle='-', color='orange', label='Write')

plt.xlabel('RAID')
plt.ylabel('Time(ms)')
plt.title('RAID Total Time Performance against Read v/s Write for Sequential Workloads')
plt.legend()
plt.grid(True)
plt.xticks(raid)
plt.savefig('stats/output/q3_c_time.png')
