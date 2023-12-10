#!/usr/bin/env python3

import matplotlib.pyplot as plt
import csv
import sys

if len(sys.argv) < 3:
    print("Usage: ./gen_time_plot.py <input_path> <output_path> <plot_name>")
    sys.exit(1)

input_path = sys.argv[1]
output_path = sys.argv[2]
plot_name = sys.argv[3]

nthreads = [1, 2, 4, 8, 16, 32, 64]

speed_up = []
efficiency = []
overhead = []
asintoto = nthreads

with open(input_path, 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    for row in lines:
            speed_up.append(float(row[2])) # speed up
            efficiency.append(float(row[3])) # efficiency
            overhead.append(float(row[4])) # overhead
                   

plt.plot(nthreads, speed_up, color='g', linestyle='dashed', marker='o', label="speed up")
plt.plot(nthreads, efficiency, color='r', linestyle='dashed', marker='o', label="efficiency")
plt.plot(nthreads, overhead, color='b', linestyle='dashed', marker='o', label="overhead")
plt.plot(nthreads, asintoto, color='y', linestyle='dashed', marker='o', label="ideal speedup")
plt.xlabel('nthreads')
plt.title(plot_name, fontsize=20)

plt.grid()
plt.legend()
image_path = f"{output_path}/{plot_name}.png"
plt.savefig(image_path)
