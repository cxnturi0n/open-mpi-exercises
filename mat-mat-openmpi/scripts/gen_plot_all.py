#!/usr/bin/env python3
import matplotlib.pyplot as plt
import csv
import sys

if len(sys.argv) < 5:
    print("Usage: ./gen_plot.py <input_path> <output_path> <x_label> <y_label> <plot_name>")
    sys.exit(1)

input_path = sys.argv[1]
output_path = sys.argv[2]
x_label = sys.argv[3]
y_label = sys.argv[4]
plot_name = sys.argv[5]

points_720_size = []
points_1008_size = []
points_1728_size = []
points_2160_size = []

points_procs_1 = []
points_procs_4 = []
points_procs_9 = []

x = [1, 4, 9] if x_label == 'nprocs' else [720, 1008, 1440, 1728, 2160] 

with open(input_path, 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    next(lines) 
    for row in lines:
          time = float(row[2]) if y_label == 'max_time' else float(row[3])
          if x_label == 'nprocs':
               input = int(row[1])
               if input == 720:
                    points_720_size.append(time)
               elif input == 1008:
                    points_1008_size.append(time)
               elif input == 1728:
                         points_1728_size.append(time)
               elif input == 2160:
                    points_2160_size.append(time)
          elif x_label == 'size':
               nprocs = int(row[0])
               if nprocs == 1:
                    points_procs_1.append(time)
               elif nprocs == 4:
                    points_procs_4.append(time)
               elif nprocs == 9:
                    points_procs_9.append(time) 

if x_label == 'nprocs':
     plt.plot(x, points_720_size, color='g', linestyle='dashed', marker='o', label="size=720x720")
     plt.plot(x, points_1008_size, color='b', linestyle='dashed', marker='o', label="size=1008x1008")
     plt.plot(x, points_1728_size, color='r', linestyle='dashed', marker='o', label="size=1728x1728")
     plt.plot(x, points_2160_size, color='y', linestyle='dashed', marker='o', label="size=2160x2160")
elif x_label == 'size':
     plt.plot(x, points_procs_1, color='g', linestyle='dashed', marker='o', label="nprocs=1")
     plt.plot(x, points_procs_4, color='b', linestyle='dashed', marker='o', label="nprocs=4")
     plt.plot(x, points_procs_9, color='r', linestyle='dashed', marker='o', label="nprocs=9")

plt.xlabel(x)
plt.ylabel(y_label)
plt.title(plot_name, fontsize=20)

plt.grid()
plt.legend()
image_path = f"{output_path}.png"
plt.savefig(image_path)