import os
import subprocess
from termcolor import colored
import matplotlib.pyplot as plt
import numpy as np
import matplotlib
matplotlib.use('Agg')
def print_cyan(str):
    print(colored(str, "cyan"))

os.system("cmake . -B build/")

os.chdir("build")
#print_cyan("make...")
os.system("make")

result = []
inter_size = range(1, 100, 2)
for i in inter_size:
    cmd = "mpirun -np " + "2" + " ./main " + f"{i}"
    output = subprocess.check_output(cmd, shell=True, text=True)
    result.append(list(map(float, "".join(output).split())))
    result[-1] = sum(result[-1])/len(result[-1])

plt.figure(figsize=[12, 5], dpi=100)
plt.plot(np.array(list(inter_size))*8*2, result, linewidth=2, label='y1')
plt.xlabel("Size, bytes")
plt.ylabel("Time, seconds")
plt.title("Dependence of interaction time on data size")
plt.minorticks_on()
plt.grid()
os.chdir("..")
current_dir = os.getcwd()
print_cyan(f"Saving graph to f{current_dir}")
#plt.savefig('graph.png')