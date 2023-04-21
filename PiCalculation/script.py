import os
import subprocess
from termcolor import colored

def print_cyan(str):
    print(colored(str, "cyan"))

import sys
if len(sys.argv) > 1:
    iters = sys.argv[1]
else:
    iters = 1000

os.system("cmake . -B build/")
os.chdir("build")
os.system("make")

print_cyan("calculating...")
cmd = "mpirun -np " + "8" + " ./main " + iters
result = subprocess.check_output(cmd, shell=True, text=True)
print(result)