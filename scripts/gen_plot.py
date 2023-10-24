import matplotlib.pyplot as plt
import csv
import sys

if len(sys.argv) < 4:
    print("Usage: python script.py <input_path> <y_label_name> <output_path> <plot_name>")
    sys.exit(1)

input_path = sys.argv[1]
y_label = sys.argv[2]
output_path = sys.argv[3]
plot_name = sys.argv[4]

nprocs = [1, 2, 4, 8]

time_strategy_I = []
time_strategy_II = []
time_strategy_III = []

with open(input_path, 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    next(lines) 
    for row in lines:
        if  y_label == 'avg_time':
            if row[2] == 'Strategy 1':
                time_strategy_I.append(float(row[4]))
            if row[2] == 'Strategy 2':
                time_strategy_II.append(float(row[4]))
            if row[2] == 'Strategy 3':
                time_strategy_III.append(float(row[4]))
        else :
            if row[2] == 'Strategy 1':
                time_strategy_I.append(float(row[5]))
            if row[2] == 'Strategy 2':
                time_strategy_II.append(float(row[5]))
            if row[2] == 'Strategy 3':
                time_strategy_III.append(float(row[5]))
                   

plt.plot(nprocs, time_strategy_I, color='g', linestyle='dashed', marker='o', label="strategy I")
plt.plot(nprocs, time_strategy_II, color='b', linestyle='dashed', marker='o', label="strategy II")
plt.plot(nprocs, time_strategy_III, color='r', linestyle='dashed', marker='o', label="strategy III")
plt.xlabel('nprocs')
plt.ylabel(y_label)
plt.title(plot_name, fontsize=20)

plt.grid()
plt.legend()
image_path = f"{output_path}/{plot_name}.png"
plt.savefig(image_path)  
