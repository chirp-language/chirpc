#!/usr/bin/env python3

"""
This script tests every file in the test folder to check if it works.
If one or more tests fail, then there is a problem with the compiler.
All tests must succed for new commits to be merged.
"""

from genericpath import isdir
import subprocess
import os
import sys

chirpc = './chirpc'
test_dir = 'tests'
suite_count = len([n for n in os.scandir(test_dir) if os.path.isdir(n)])

def run_command(*args):
    try:
        return subprocess.run(args, stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL).returncode
    except:
        return -1

if run_command(chirpc, '--help') != 0:
    print("Cannot run test. chirpc executable cannot be found")
    sys.exit(-1)

print('Running Chirpc Tests.')

i = 1
for suite in os.scandir(test_dir):
    if not suite.is_dir():
        continue
    print(f"-- Testing suite: {suite.name} ({i}/{suite_count}) --")
    i += 1
    for test in os.scandir(suite.path):
        print(f'>  Running test: {test.name}')
        cmd = chirpc, test.path
        if run_command(*cmd) != 0:
            print(f"\"{test.name}\" Test failed (command: {' '.join(map(repr, cmd))})")
            sys.exit(-1)

print('\nAll tests succeded!')
