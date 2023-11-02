import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV data into a pandas DataFrame
data = pd.read_csv('stats/input/q3_a.csv')  # Replace 'your_file.csv' with your actual file name

# Plotting the data
plt.figure(figsize=(10, 6))  # Adjust the figure size if needed

# Iterate through each RAID level and plot the corresponding data
for raid_level in data['RAID']:
    plt.plot(data.columns[1:], data[data['RAID'] == raid_level].values.flatten()[1:], marker='o', label=f'RAID {raid_level}')

plt.xlabel('Number of Disks')
plt.ylabel('Total Time(ms)')
plt.title('RAID Performance Comparison on Random Reads')
plt.legend()
plt.grid(True)
plt.savefig('stats/output/q3_a.png')
