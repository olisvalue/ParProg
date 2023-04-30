import os
import sys
import subprocess
from termcolor import colored
import matplotlib.pyplot as plt
import numpy as np
import matplotlib
matplotlib.use('Agg')

def print_cyan(str):
    print(colored(str, "cyan"))


def makeplot(x, y, xlabel = "x", ylabel = "y", title = "y(x)", show = 1, grid = 1, label = 0):
    if label != 0:
        plt.plot(x, y, label = label, color = np.random.rand(3))
        plt.legend(loc="upper right", bbox_to_anchor=(1.4, 0.95))
    else:
        plt.plot(x, y, color = "b")
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    if grid:
        plt.grid()
    plt.show()

time = [[] for _ in range(8)]
time[0] = [2.116253, 2.492626, 2.799019, 2.482158, 2.452798]
time[1] = [1.481212, 1.637023, 1.422093, 1.781793, 1.650752]
time[2] = [1.569710, 1.363960, 1.608525, 1.600926, 1.462890, 1.229013]
time[3] = [1.176755, 1.396583, 1.283754, 1.290373, 1.252033, 1.321007, 0.916616]
time[4] = [1.128481, 1.048503, 1.130429, 1.273135, 1.164692, 1.242155, 0.761239]
time[5] = [1.174867, 0.945139, 1.027910, 1.192271, 0.699845]
time[6] = [1.033784, 1.171423, 0.889097, 1.018922]
time[7] = [1.017762, 0.980433, 0.962912]
for i in range(8):
    time[i] = sum(time[i])/len(time[i])
acc = []
for i in range(1, 8):
    acc.append(time[0]/time[i])
eff = []
for i in range(7):
    eff.append(acc[i]/(i+2))
makeplot(range(2,9), eff, title = "Efficiency", xlabel = "Num", ylabel = "acceleration / parallel time")

current_dir = os.getcwd()
print_cyan(f"Saving graph to f{current_dir}")
plt.savefig('Efficiency.png')