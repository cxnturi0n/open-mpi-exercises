#!/usr/bin/env python3

import matplotlib.pyplot as plt
import csv
import sys

if len(sys.argv) < 3:
    print("Usage: ./gen_time_plot.py <input_path> <output_path>  <plot_name>")
    sys.exit(1)

input_path = sys.argv[1]
output_path = sys.argv[2]
plot_name = sys.argv[3]

x = [1, 4, 9]

speed_up = []
efficiency = []
overhead = []


with open(input_path, 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    next(lines)
    for row in lines:
            speed_up.append(float(row[3])) # speed up
            efficiency.append(float(row[4])) # efficiency
            overhead.append(float(row[5])) # overhead
                   

plt.plot(x, speed_up, color='g', linestyle='dashed', marker='o', label="speed up")
plt.plot(x, efficiency, color='r', linestyle='dashed', marker='o', label="efficiency")
plt.plot(x, overhead, color='b', linestyle='dashed', marker='o', label="overhead")
plt.xlabel('nprocs')
plt.title(plot_name, fontsize=20)

plt.grid()
plt.legend()
image_path = f"{output_path}.png"
plt.savefig(image_path)