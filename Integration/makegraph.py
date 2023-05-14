import os
import sys
import subprocess
import matplotlib.pyplot as plt
import numpy as np
import matplotlib
matplotlib.use('Agg')


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


def calculate_acc(time, n_threads):
    acc = []
    for i in range(n_threads):
        acc.append(time[0]/time[i])
    return acc


def calculate_eff(time, acc, n_threads):
    eff = []
    for i in range(n_threads):
        eff.append(acc[i]/(i+1))
    return eff

args = sys.argv
if len(args) != 3:
    print("Too few arguments!")
    exit()
else:
    n_threads = int(args[1])
    accuracy = args[2]

os.system("cmake . -B build/")
os.chdir("build")
os.system("make")

#number of rerunning for each threads number, to get mean representative value
num_rerun = 5

time = [0]*n_threads
for i in range(1, n_threads+1):
    result = []
    cmd = "./main " + str(i) + " " + str(accuracy)
    for j in range(num_rerun):
        output = subprocess.check_output(cmd, shell=True, text=True).split()
        result.append(float(output[0]))
    time[i-1] = sum(result)/len(result)

acc = calculate_acc(time, n_threads)
eff = calculate_eff(time, acc, n_threads)

os.chdir("..")
os.system("rm -rf build")
current_dir = os.getcwd()
path = current_dir + "/imgs"
if not os.path.exists(path):
    os.makedirs(path)
print(f"Saving graph to f{path}")
os.chdir(path)



makeplot(range(1, n_threads+1), time, title = "Time", xlabel = "Threads number", ylabel = "time, s")
plt.savefig('Time.png')
plt.clf()
makeplot(range(1, n_threads+1), acc, title = "Acceleration", xlabel = "Threads number", ylabel = "parallel/sequential time")
plt.savefig('Acceleration.png')
plt.clf()
makeplot(range(1, n_threads+1), eff, title = "Efficiency", xlabel = "Threads number", ylabel = "acceleration/threads number")
plt.savefig('Efficiency.png')
plt.clf()



