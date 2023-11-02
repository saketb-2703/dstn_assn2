import pandas as pd

# Load the CSV data into a pandas DataFrame
data = pd.read_csv('stats/input/q3_a.csv')  # Replace 'your_file.csv' with your actual file name

# Create a Pandas DataFrame from the CSV data
df = pd.DataFrame(data)

# Write the DataFrame to an Excel file
output_file = 'stats/output/q3.xlsx'  # The name of the output Excel file
df.to_excel(output_file, index=False)  # Change index to True if you want to include the row numbers

print(f"Excel file '{output_file}' has been created.")
