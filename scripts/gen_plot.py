import matplotlib.pyplot as plt
import numpy as np
import csv
import sys

filename = sys.argv[1]
time_index = 5 if len(sys.argv) == 2 else 4
plot_name = "plot " + filename[6:9]

y_label = "time " + ("max" if time_index == 5 else "avg")

nprocs = [1,2,4,8]
time_max_strategy_I = []
time_max_strategy_II = []
time_max_strategy_III = []

with open(filename, 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter = ',')
    for row in lines:
        if row[2] == 'Strategy 1':
            time_max_strategy_I.append(float(row[4]))
        if row[2] == 'Strategy 2':
            time_max_strategy_II.append(float(row[4]))
        if row[2] == 'Strategy 3':
            time_max_strategy_III.append(float(row[4]))

plt.plot(nprocs, time_max_strategy_I, color = 'g', linestyle='dashed', marker='o', label="strategy I")
plt.plot(nprocs, time_max_strategy_II, color = 'b', linestyle='dashed', marker='o', label="strategy II")
plt.plot(nprocs, time_max_strategy_III, color = 'r', linestyle='dashed', marker='o', label="strategy III")
plt.xlabel('nprocs')
plt.ylabel(y_label)
plt.title(plot_name, fontsize = 20)

plt.grid()
plt.legend()
plt.show()
