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


def calculate_sol(arg1, arg2):
    os.system("mpicc consistent_sol.c -lm")
    cmd = "mpirun -np " + "1" + " ./a.out " + f"{arg1} " + f"{arg2}"
    subprocess.run(cmd, shell=True, check=True)
    u = np.genfromtxt('data.csv', delimiter=',')
    k = u.shape[0]
    n = u.shape[1]
    ts = np.linspace(0, 1, k)
    xs = np.linspace(0, 1, n)
    return u, ts, xs


def make_surface(u, t, x, title=''):
    fig = plt.figure(figsize=(14, 7))
    ax = plt.axes(projection='3d')
    ts, xs = np.meshgrid(t, x)
    ax.plot_surface(ts, xs, u.transpose(), rstride=1, cstride=1, cmap='jet', edgecolor='none')
    ax.view_init(20, -130)
    if title != '':
        ax.set_title(title, fontsize=13)
    ax.set_xlabel('t', fontsize=11)
    ax.set_ylabel('x', fontsize=11)
    ax.set_zlabel('u(t, x)', fontsize=10)


args = sys.argv
if len(args) < 3:
    arg1 = arg2 = 50
else:
    arg1 = args[1]
    arg2 = args[2]
u, ts, xs = calculate_sol(arg1, arg2)
make_surface(u, ts, xs, title="Solution")
current_dir = os.getcwd()
name = 'solution_rec.png'
print_cyan(f"Saving graph to f{current_dir+'/'+name}")
plt.savefig('solution_rec.png')