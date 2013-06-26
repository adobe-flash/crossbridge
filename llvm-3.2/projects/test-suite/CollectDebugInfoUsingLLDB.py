#!/usr/bin/python

#----------------------------------------------------------------------
# 
# Be sure to add the python path that points to the LLDB shared library.
# On MacOSX csh, tcsh:
#   setenv PYTHONPATH /Developer/Library/PrivateFrameworks/LLDB.framework/Resources/Python
# On MacOSX sh, bash:
#   export PYTHONPATH=/Developer/Library/PrivateFrameworks/LLDB.framework/Resources/Python
#
# This script collect debugging information using LLDB. This script is
# used by TEST=dbg in llvm testsuite to measure quality of debug info in
# optimized builds.
#
# Usage:
# export PYTHONPATH=...
# ./CollectDebugInfUsingLLDB.py program bp_file out_file
#     program - Executable program with debug info.
#     bp_file - Simple text file listing breakpoints.
#               <absolute file name> <line number>
#     out_file - Output file where the debug info will be emitted.
#----------------------------------------------------------------------

import lldb
import os
import sys
import time

# AlreadyPrintedValues - A place to keep track of recursive values.
AlreadyPrintedValues = {}

# ISAlreadyPrinted - Return true if value is already printed.
def IsAlreadyPrinted(value_name):
        if AlreadyPrintedValues.get(value_name) is None:
                AlreadyPrintedValues[value_name] = 1
                return False
        return True


# print_var_value - Print a variable's value.
def print_var_value (v, file, frame):
        if v.IsValid() == False:
                return
        if IsAlreadyPrinted(v.GetName()):
                return
        total_children = v.GetNumChildren()
        if total_children > 0:
            c = 0
            while (c < total_children) :
                    child = v.GetChildAtIndex(c)
                    if child is None:
                        file.write("None")
                    else:
                        if (child.GetName()) is None:
                                file.write("None")
                        else:
                                file.write(child.GetName())
                                file.write('=')
                                print_var_value(child, file, frame)
                                file.write(',')
                    c = c + 1
        else:
            if v.GetValue(frame) is None:
                file.write("None")
            else:
                file.write(v.GetValue(frame))

def disable_bp(thread):
    # disable this thread.
    count = thread.GetStopReasonDataCount()
    bid = 0
    tid = 0
    for i in range(count):
        id = thread.GetStopReasonDataAtIndex(i)
        bp = target.FindBreakpointByID(id)
        if bp.IsValid():
            if bp.IsEnabled() == True:
                    bid = bp.GetID()
                    tid = bp.GetThreadID()
                    bp.SetEnabled(False)
	            # print " disabled [", str(bp.GetThreadID()), ":", str(bp.GetID()), "]"
        else:
            bp_loc = bp.FindLocationByID(thread.GetStopReasonDataAtIndex(i+1))
            if bp_loc.IsValid():
                bid = bp_loc.GetBreakPoint().GetID()
                tid = bp_loc.ThreadGetID()
                bp_loc.SetEnabled(False);
	        # print " disabled [", str(bp.GetThreadID()), ":", str(bp.GetID()), "]"

# print_vars - Print variable values in output file.
def print_vars (tag, vars, fname, line, file, frame, target, thread):

    bid = 0
    tid = 0
    count = thread.GetStopReasonDataCount()
    # print "count = ",count
    for i in range(count):
        # print "i =", i
        id = thread.GetStopReasonDataAtIndex(i)
        bp = target.FindBreakpointByID(id)
        if bp.IsValid():
		bid = bp.GetID()
		tid = bp.GetThreadID()
                # print "bp is valid", bid, tid
		for j in range(vars.GetSize()):
			v = vars.GetValueAtIndex(j)
			if v.GetName() is not None:
				file.write(tag)
				file.write(fname)
				file.write(':')
				file.write(str(line))
				file.write(' ')
				file.write(str(tid))
				file.write(':')
				file.write(str(bid))
				file.write(' ')
				file.write(v.GetName())
				file.write(' ')
				AlreadyPrintedValues.clear()
				print_var_value (v, file, frame)
				file.write('\n')

# set_breakpoints_old - set breakpoints as listed in input file.
def set_breakpoints_old (target, breakpoint_filename, file):
    f = open(breakpoint_filename, "r")
    lines = f.readlines()
    for l in range(len(lines)):
        c = lines[l].split()
        # print "setting break point - ", c
        bp = target.BreakpointCreateByLocation (str(c[0]), int(c[1]))
        file.write("#Breakpoint ")
        file.write(str(c[0]))
        file.write(':')
        file.write(str(c[1]))
        file.write(' ')
        file.write(str(bp.GetThreadID()))
        file.write(':')
        file.write(str(bp.GetID()))
        file.write('\n')
    f.close()

# stopeed_at_breakpoint - Return True if process is stopeed at a
# set_breakpoints - set breakpoints as listed in input file.
def set_breakpoints (target, breakpoint_filename, file):
    f = open(breakpoint_filename, "r")
    lines = f.readlines()
    for l in range(len(lines)):
	l2 = len(lines[l])
	l3 = l2 - 1
        # print "setting break point - ", lines[l][0:l3]
        bp = target.BreakpointCreateByName (str(lines[l][0:l3]))
        # print "setting break point - ", lines[l][0:l3],
	# print " [", str(bp.GetThreadID()), ":", str(bp.GetID()), "]"
        file.write("#Breakpoint ")
        file.write(str(1))
        file.write(':')
        file.write(str(2))
        file.write(' ')
        file.write(str(bp.GetThreadID()))
        file.write(':')
        file.write(str(bp.GetID()))
        file.write(' ')
        file.write(str(lines[l][0:l3]))
        file.write('\n')
    f.close()

# stopeed_at_breakpoint - Return True if process is stopeed at a
# breakpoint.
def stopped_at_breakpoint (process):
    # print "stopped"
    if process.IsValid():
        # print "stopped process"
        state = process.GetState()
        # print "stopped process", state
        if state == lldb.eStateStopped:
                # print "stopped process state is stopped"
                thread = process.GetThreadAtIndex(0)
                if thread.IsValid():
                        # print "thread is valid"
                        if thread.GetStopReason() == lldb.eStopReasonBreakpoint:
                        	# print "thread stopped at breakpoint"
                                return True
    return False

# Create a new debugger instance
debugger = lldb.SBDebugger.Create()

# When we step or continue, don't return from the function until the process 
# stops. We do this by setting the async mode to false.
debugger.SetAsync (False)

# Create a target from a file and arch
# print "Creating a target for '%s'" % sys.argv[1]

target = debugger.CreateTargetWithFileAndArch (sys.argv[1], lldb.LLDB_ARCH_DEFAULT)

if target.IsValid():
    # print "target is valid"
    file=open(str(sys.argv[3]), 'w')    
    set_breakpoints (target, sys.argv[2], file)

    # Launch the process. Since we specified synchronous mode, we won't return
    # from this function until we hit the breakpoint at main
    sberror = lldb.SBError()
    process = target.LaunchSimple(None, None, None)
    # Make sure the launch went ok
    while stopped_at_breakpoint(process):
        # print "stopped at a bp"
        thread = process.GetThreadAtIndex (0)
	# print "num of frames ", thread.GetNumFrames()
        frame = thread.GetFrameAtIndex (0)
        if not frame.IsValid():
		for fi in range(thread.GetNumFrames()):
			# print "checking frame no : ", fi
			frame = thread.GetFrameAtIndex(fi)
			if frame.IsValid():
				fi = thread.GetNumFrames()
        if frame.IsValid():
            # #Print some simple frame info
            ##print frame
            # print "frame is valid"
            function = frame.GetFunction()
            if function.IsValid():
                fname = function.GetMangledName()
                if fname is None:
                    fname = function.GetName()
                # print "function : ",fname
                line = frame.GetLineEntry().GetLine()
                vars = frame.GetVariables(1,0,0,0)
                print_vars ("#Argument ", vars, fname, line, file, frame, target, thread)
                # vars = frame.GetVariables(0,1,0,0)
                # print_vars ("#Variables ", vars, fname, line, file, frame, target, thread)
	disable_bp(thread)
        process.Continue()
    file.close()

lldb.SBDebugger.Terminate()
