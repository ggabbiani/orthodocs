#!/usr/bin/env python3

"""
executes the list of passed os commands.
stop after last command or at first rc!=0
"""
import argparse, subprocess

parser  = argparse.ArgumentParser()

parser.add_argument(
    "commands", # name on the CLI - drop the `--` for positional/required parameters
    nargs="*",  # 0 or more values expected => creates a list
    type=str
)

args = parser.parse_args()

print("strings: %r" % args.commands)
for c in args.commands:
    rc = subprocess.call(c,shell=True)
    if rc!=0:
        raise SystemExit("command '%s' returned %i" % (c,rc))
