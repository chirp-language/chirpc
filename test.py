# This scripts tests every files in the test folder to check if they work.
# If one or more test fails, then there is a problem with the compiler.
# All tests must succed for new commits to be merged.

from genericpath import isdir
import os
import sys

chirpc = './chirpc'
test_dir = 'tests'
test_count = len([n for n in os.scandir(test_dir) if os.path.isdir(n)])

if os.system(chirpc + " >/dev/null 2>&1") == 32512:
    print("Cannot run test. chirpc executable cannot be found")
    sys.exit(-1)

print('Running Chirpc Tests.')

i = 1
for entry in os.scandir(test_dir):
    if entry.is_dir():
        print(f"-- Testing: {entry.name} ({i}/{test_count}) --")
        i += 1
        cmd = f"{chirpc} {[n for n in os.scandir(test_dir + '/' + entry.name)][0].path} >/dev/null 2>&1"
        if os.system(cmd) != 0:
            print(f"\"{entry.name}\" Test failed")
            sys.exit(-1)