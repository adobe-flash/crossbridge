#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

# display the heap map or not, set this to true to get the mpa of used/unused/reserved pages
displayHeap = False;

# number of types to print in each namespace
kNumTypes = 10;

# number of traces to print for each type
kNumTracesPerType = 5;

# known namespace names - the mapfile apparently does not differentiate between
# class names and namespaces names in any way, avmplus::MyFunc and MyClass::MyFunc 
# are mangled the same way.
ns_names = ["avmplus", "MMgc", "nanojit"];

def usage():
	print "Usage: python winmo-stacktraces.py <mapfile> <stacktrace file>";

# calculate the offset to get relative address from the absolute addresses in the trace file
def calcoffset(abs_addr, rel_addr):
	diff = (abs_addr - rel_addr)/0x10000;
	diff *= 0x10000;
	return diff;
	
# special names - list of names we will encounter in a MSVC++ generate map file.  These
# are always preceded by ? and mean special things, i.e.
# ?_U == operator new []
special_names= {}
special_names["0"] = "ctor";
special_names["1"] = "dtor";
special_names["2"] = "operator new";
special_names["3"] = "operator delete";
special_names["4"] = "operator->";
special_names["5"] = "operator*";
special_names["6"] = "operator&";
special_names["7"] = "operator->*";
special_names["_U"] = "operator new[]";
special_names["_V"] = "operator delete[]";


def demangleName(str):
	end_index = str.find("@@"); # don't care about function args
	name = str[0:end_index];
	start_index = 0;
	spec_name = "";
	while name[start_index] == "?":
		start_index += 1;
		char = name[start_index];
		if char == "?":
			continue;
		elif char == "_":
			start_index += 1;
			char += name[start_index];
			
		if special_names.has_key(char):
			spec_name = special_names[char];
			start_index += 1;
	
	name = name[start_index:len(name)];
	
	splits = name.split("@");

	i = 0;
	if spec_name != "":
		func_part = spec_name;
	else:
		func_part = splits[0];
		i += 1;
		
	l = len(splits);
	
	class_part = "";
	ns_part = "";
	while i < l:
		part = splits[i];
		if i == l-1 and part in ns_names:
			ns_part = part;
		elif class_part != "":
			class_part += "::"+part;
		else:
			class_part = part;
		i +=1;
	if ns_part =="":
		ns_part = "global";
		
	return dict(func=func_part, clss=class_part, namespace=ns_part);
	
# returns a dictionary representing the named map file:
# {
#   offset - the offest to translate absolute addresses into relative addresses
#   addrs - an ordered list of [addr, name, object file].  One for each entyr in the map file
# }
def readMapFile(filename, refaddr):
	mapfile = open(filename, "r");
	ln = mapfile.readline();
	while ln.startswith("  Address") == False:
		ln = mapfile.readline();
	
	addrs = [];
	
	refname = refaddr["name"];
	while ln != "":
		ln = mapfile.readline();
		splits = ln.split();
		if len(splits) > 3 and splits[0].startswith("00"):
			name = demangleName(splits[1]);
			if name["func"].startswith(refname):
				offset = calcoffset(refaddr["addr"], int(splits[2], 16));
				
			addrs.append([int(splits[2], 16), name, splits[(len(splits)-1)] ]);

	return dict(offset=offset, addrs=addrs);

# returns a dictionary:
# {
#   header - any data before the stack traces
#	refaddr - name & address of a reference symbol - used to calculate relative addresses
#   traces[] - list of stack traces
# }
def readStackFile(stackFileName):
	stackfile = open(stackFileName, "r");
	ln = stackfile.readline();
	splits = ln.split();
	
	stacks = {};
	stacks["header"] = [];
	stacks["traces"] = [];

	save_header = True;
	while len(splits) != 6 or (splits[1] != "b" and splits[4] != "items") :
		
		if ln.startswith("ReferenceAddress"):
			refaddr = {};
			refaddr["name"] = splits[1];
			refaddr["addr"] = int(splits[2], 16);
			stacks["refaddr"] = refaddr;
		elif save_header:	
			stacks["header"].append(ln.strip());

		if displayHeap == False and ln.find("gross stats end") != -1:
			save_header = False;

		ln = stackfile.readline();
		splits = ln.split();
		
	trace_count = 0;
	while ln != "":
		if len(splits) == 6 and splits[1] == "b" and splits[4] == "items":
			stack = {};
			stack["size"] = int(splits[0]);
			stack["items"] = int(splits[3]);
			
			trace = [];
			ln = stackfile.readline();
			#skip any garbage before we get to the stacks
			while ln.startswith("0x") == False:
				ln = stackfile.readline();
			while ln.startswith("0x") == True:
				trace.append(int(ln, 16));
				ln = stackfile.readline();
				
			stack["trace"] = trace;
			stacks["traces"].append(stack);
			trace_count+=1;

		ln = stackfile.readline();
		splits = ln.split();

	return stacks;
	
addr_table = {};
def findAddr(map, addr):
	
	if addr in addr_table:
		return addr_table[addr];
	
	offset = map["offset"];
	addr_list = map["addrs"];
	
	rel_addr = addr - offset;
	if rel_addr < 0:
		addr_table[addr] = [addr, dict(func=str(hex(addr)), clss="", namespace=""), addr]
		return addr_table[addr];
		
	low = 0;
	hi = len(addr_list)-1;
	index = -1;
	while low <= hi and index == -1:
		pivot = (low+hi)/2;
		# addr_list should contain [addr, function name, object file]
		# as assembled above in readMapFile
		test_addr = addr_list[pivot][0];
		if rel_addr < test_addr:
			hi = pivot;
		elif rel_addr > test_addr:
			if addr_list[pivot+1][0] > rel_addr:
				index = pivot;
			low = pivot;
		else:
			index = pivot;
	
	if index != -1:
		addr_table[addr] = addr_list[index];
	return addr_table[addr];
	

def translateTraces(traces, addrs):
	for t in traces:
		trace = t["trace"];
		new_trace = [];
		for item in trace:
			addrinfo = findAddr(addrs, item);
			if str(addrinfo[1]["func"]).startswith(("Alloc", "GetStackTrace", "LargeAlloc", "Calloc", "operator new", "RecordAlloc")) == False:
				new_trace.append(addrinfo[1]);
		t["trace"] = new_trace;

	# dont need to keep this around
	addr_table = {};
	
def cmpBySize(a, b):
	a_size = a["size"];
	b_size = b["size"];
	return b_size-a_size;
	
# sort the traces
# Sorts the traces into their different namespaces, and sorts them in descending order in each namespace.
# the namespaces are also sorted in descending order based on the total of all allocations in that namespace
# sorted trace:
# [namespace1, namespace2, ... namespaceN]
# Each namespace entry is a dictionary:
# {
#	name - namespace name
#   size - bytes alloc'ed
#   count - number of items
#   traces[] - sorted list of traces
# }
def sortTraces(traces):
	namespaces = {};
	for t in traces:
		stack = t["trace"];
		size = t["size"];
		count = t["items"];
		
		ns = stack[0]["namespace"];
		name = stack[0]["clss"] + "::" + stack[0]["func"] + "()";
		
		if namespaces.has_key(ns) == False:
			namespaces[ns] = dict(name=ns, size=0, count=0, traces={});
			
		ns_entry = namespaces[ns];
		ns_entry["size"] += size;
		ns_entry["count"] += count;
		ns_traces = ns_entry["traces"];
		if ns_traces.has_key(name) == False:
			ns_traces[name] = dict(name=name, size=0, count=0, traces=[]);
		ns_trace = ns_traces[name];
		ns_trace["size"] += size;
		ns_trace["count"] += count;
		ns_trace["traces"].append(t);
		
	ns_list = [];
	
	for k,v in namespaces.items():
		ns_list.append(v);
	
	# sort the namespaces by size
	ns_list.sort(cmpBySize);
	
	for ns in ns_list:
		traces = ns["traces"];
		traces_list = [];
		for k,v in traces.items():
			# sort the traces for each function
			v["traces"].sort(cmpBySize);
			traces_list.append(v);
		# sort the functions
		traces_list.sort(cmpBySize);
		ns["traces"] = traces_list;
		
	return ns_list;
	
def printTrace(trace, total_size):
	trace_size = trace["size"];
	trace_count = trace["items"];
	print "\t\t" + str(round(float(trace_size)/total_size*100, 2)) + "% - " + str(trace_size/1024) + " kb - " + str(trace_count) + " items";
	for t in trace["trace"]:
		print "\t\t\t" + str(t["clss"]) + "::" + str(t["func"]);
		
def printSortedTraces(l):
	allocs = 0;
	bytes = 0;
		
	for ns in l:
		allocs += ns["count"];
		bytes += ns["size"];
		
	line = "Memory allocation report for " + str(allocs) + " allocations, totalling " + str(bytes/1024) + " kb (" + str(bytes/allocs) + " ave) across " + str(len(l)) + " packages";
	print line;
	
	for ns in l:
		line = ns["name"];
		ns_size = ns["size"];
		ns_count = ns["count"];
		line += " - " + str(round(float(ns_size)/bytes*100, 2)) + "% - " + str(ns_size/1024) + " kb " + str(ns_count) + " items, avg " + str(ns_size/ns_count) + " b";
		print line;
		
		traces = ns["traces"];
		numtypes = 0;
		for t in traces:
			if numtypes == kNumTypes:
				break;
			numtypes += 1;
			tg_size = t["size"];
			tg_count = t["count"];
			line = "\t" + t["name"] + " - " + str(round(float(tg_size)/bytes*100, 2)) + "% - " + str(tg_size/1024) + " kb " + str(tg_count) + " items, avg " + str(tg_size/tg_count) + " b";
			print line;
			sub_traces = t["traces"];
			numtraces = 0;
			for f in sub_traces:
				if numtraces == kNumTracesPerType:
					break;
				numtraces += 1;
				printTrace(f, tg_size);
				print "";
		
import sys;

if len(sys.argv) != 3:
	usage();
	sys.exit();
	


stacks = readStackFile(sys.argv[2]);

addrs = readMapFile(sys.argv[1], stacks["refaddr"]);

translateTraces(stacks["traces"], addrs);
sorted_traces = sortTraces(stacks["traces"]);

header = stacks["header"];
for ln in header:
	print ln;
print "";

printSortedTraces(sorted_traces);
