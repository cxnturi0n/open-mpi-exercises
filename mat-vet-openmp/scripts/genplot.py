#!/usr/bin/env python3

import matplotlib.pyplot as plt
import csv
import sys

if len(sys.argv) < 4:
    print("Usage: ./gen_plot.py <input_path> <output_path> <plot_name> <label_name>")
    sys.exit(1)

input_path = sys.argv[1]
output_path = sys.argv[2]
plot_name = sys.argv[3]
label_name = sys.argv[4]

nthreads = [1, 2, 4, 8, 16, 32, 64]

points = []

with open(input_path, 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    for row in lines:
            points.append(float(row[1]))
                   

plt.plot(nthreads, points, color='g', linestyle='dashed', marker='o', label=label_name)
plt.xlabel('nthreads')
plt.ylabel('time')
plt.title(plot_name, fontsize=20)

plt.grid()
plt.legend()
image_path = f"{output_path}/{plot_name}-{label_name}.png"
plt.savefig(image_path)
