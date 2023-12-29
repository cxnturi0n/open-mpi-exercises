#!/usr/bin/env python3
import matplotlib.pyplot as plt
import csv
import sys

if len(sys.argv) < 6:
    print("Usage: ./gen_plot.py <input_path> <output_path> <x_label> <y_label> <plot_name> <label_name>")
    sys.exit(1)

input_path = sys.argv[1]
output_path = sys.argv[2]
x_label = sys.argv[3]
y_label = sys.argv[4]
plot_name = sys.argv[5]
label_name = sys.argv[6]

x = [1, 4, 9] if x_label == 'nprocs' else [720, 1008, 1440, 1728, 2160] 

points = []

with open(input_path, 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    next(lines) 
    for row in lines:
            time = float(row[1]) if y_label == 'max_time' else float(row[2])
            points.append(float(row[1]))
                   

plt.plot(x, points, color='g', linestyle='dashed', marker='o', label=label_name)
plt.xlabel(x_label)
plt.ylabel(y_label)
plt.title(plot_name, fontsize=20)

plt.grid()
plt.legend()
image_path = f"{output_path}.png"
plt.savefig(image_path)