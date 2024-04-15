import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file into a pandas DataFrame
df = pd.read_csv('ExecutionTimes.csv')

# Filter out unnecessary columns
df = df[['Class/Operation', 'Method', 'Data', 'Processes', 'ROUND(Speedup)']]

# Group the data by "Class/Operation" and "Method"
groups = df.groupby(["Class/Operation", "Method"])

# Define colors for different processes
process_colors = {2: 'blue', 4: 'orange', 8: 'red'}

# Plot each group separately
for name, group in groups:
    title, subtitle = name
    plt.figure()
    plt.title(title)
    plt.suptitle(subtitle)
    for process, data in group.groupby("Processes"):
        plt.plot(data["Data"], data["ROUND(Speedup)"], label=f"Processes: {process}", color=process_colors.get(process, 'green'))
    plt.xlabel("Data")
    plt.ylabel("Speedup")
    plt.legend(title="Processes")
    plt.grid(True)
    plt.show()

