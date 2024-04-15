import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file into a DataFrame
df = pd.read_csv('ExecutionTimes.csv', skiprows=lambda x: x == 1 or x == 2 or x == 3)

# Filter out unnecessary columns
df = df[['Class/Operation', 'Method', 'Data', 'Proccesses', 'ROUND(Speedup)']]
# Get unique methods
methods = df['Method'].unique()

# Plot for each method
for method in methods:
    method_df = df[df['Method'] == method]
    plt.figure(figsize=(10, 6))
    plt.title(f'Speedup for {method}')
    processes = method_df['Proccesses'].unique()
    for process in processes:
        process_df = method_df[method_df['Proccesses'] == process]
        subtitle = process_df['Class/Operation'].iloc[0]  # Get the subtitle from the first row of the group
        plt.plot(process_df['Data'], process_df['ROUND(Speedup)'], label=f'Processes: {process}')
    plt.xlabel('Number of Data')
    plt.ylabel('ROUND(Speedup)')
    plt.legend()
    plt.xscale('log')
    plt.xticks(method_df['Data'].unique(), rotation=45)
    plt.grid(True)
    plt.tight_layout()
    plt.show()
