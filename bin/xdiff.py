#!/usr/bin/env python3

"""
executes the passed command, then execute diff.py on the passed source and 
destination directories
"""

import sys, shlex, argparse, codecs, os, subprocess, filecmp

from subprocess import PIPE, Popen
from filecmp import dircmp

def pack_cmd(cmd, arg_list):
    """
    pack cmd with passed argument list into a string list
    """
    args = [cmd]
    args.extend(arg_list)
    return args

def print_diff_files(dcmp):
    for name in dcmp.diff_files:
        print("diff_file %s found in %s and %s" % (name, dcmp.left,dcmp.right))
        Popen([file_diff,'-u','-l','0',os.path.join(dcmp.left,name),os.path.join(dcmp.right,name)]).communicate()
    for sub_dcmp in dcmp.subdirs.values():
        print_diff_files(sub_dcmp)

# builds the default diff command
file_diff    = os.path.join(os.path.abspath(os.path.join(os.path.realpath(__file__), os.pardir)), 'diff.py')

parser  = argparse.ArgumentParser(prefix_chars='-+')
parser.add_argument('++left',type=str,  required=True,  help='existent document root')
parser.add_argument('++right',type=str, required=True,  help='test document root')
parser.add_argument('command',type=str, help='command to be executed')

options,rest = parser.parse_known_args()

# pre-command
process     = Popen(pack_cmd(options.command,rest), stdout=PIPE, stderr=PIPE)
out, err    = process.communicate()
print(codecs.decode(out))
if process.returncode>0:
    print(codecs.decode(err))
    exit(process.returncode)

# directory contents comparison
dcmp = dircmp(options.left,options.right) 
print_diff_files(dcmp) 
exit(len(dcmp.diff_files)>0)