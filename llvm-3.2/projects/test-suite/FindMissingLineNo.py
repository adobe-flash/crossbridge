#!/usr/bin/python

import os
import sys

DBG_OUTPUT_FILE="Output/" + sys.argv[1] + ".dbg.ml"
OPT_DBG_OUTPUT_FILE="Output/" + sys.argv[1] + ".dbg.opt.ml"
NATIVE_DBG_OUTPUT_FILE="Output/" + sys.argv[1] + ".native.dbg.ml"
NATIVE_OPT_DBG_OUTPUT_FILE="Output/" + sys.argv[1] + ".native.dbg.opt.ml"
XFAIL_FILE=sys.argv[2] + "/lineinfo.xfail"

REPORT_FILE="Output/" + sys.argv[1] + ".dbg.missing_lines.report.txt"

def read_inputfile(filename, dict):
    f = open(filename, "r")
    lines = f.readlines()
    for l in range(len(lines)):
        columns = lines[l].split()
        s = dict.get(columns[0])
        if s is None:
            s = set()
        s.add(columns[1])
        dict[columns[0]] = s
    f.close()
    return

dbg_lines = {}
read_inputfile(DBG_OUTPUT_FILE, dbg_lines)

dbg_opt_lines = {}
read_inputfile(OPT_DBG_OUTPUT_FILE, dbg_opt_lines)

native_dbg_lines = {}
read_inputfile(NATIVE_DBG_OUTPUT_FILE, native_dbg_lines)

native_dbg_opt_lines = {}
read_inputfile(NATIVE_OPT_DBG_OUTPUT_FILE, native_dbg_opt_lines)

xfailed_lines = {}
read_inputfile(XFAIL_FILE, xfailed_lines)

dbg_line_items = dbg_lines.items()
for f in range(len(dbg_line_items)):
    fname = dbg_line_items[f][0]
    fset = dbg_line_items[f][1]
    optset = dbg_opt_lines.get(fname)
    nativeoptset = native_dbg_opt_lines.get(fname)
    xfailedset = xfailed_lines.get(os.path.basename(fname))
    if optset is not None:
        src = open(fname, "r")
        srclines = src.readlines()
        src_output = open("Output/" + sys.argv[1] + ".lineinfo", "w")
        for l in range(len(srclines)):
            l1 = l + 1
            l1s = str(l1)
            if l1s in fset:
                if l1s in optset:
                    if nativeoptset is not None and l1s in nativeoptset:
                        src_output.write(".|")
                    else:
                        src_output.write("+|")
                else:
                    if nativeoptset is not None and l1s in nativeoptset:
                        if xfailedset is not None and l1s in xfailedset:
                            src_output.write(" |")
                        else:
                            src_output.write("x|")
                    else:
                        src_output.write("-|")
            else:
                src_output.write(" |")
            src_output.write(srclines[l])
        src.close()
        src_output.close()
