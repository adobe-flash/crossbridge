-- run benchmark scripts.
--
local write = io.write
local use_profiler = false
local profile_mode = 2
local profile_all = true
local prof_start = function(file,mode) end
local prof_stop = function() end
local output_fd = io.stdout
local old_print = print
local script_print = old_print
--local run_type = (arg and arg[1]) or "bench"
local run_type="test"

local function redirect_print(...)
	for i,val in ipairs(...) do
		write(val)
		write("\t")
	end
	write("\n")
end

if use_profiler then
	require("profiler")
	if profile_all then
		profiler.start("Stats.log", profile_mode)
	else
		prof_start = profiler.start
		prof_stop = profiler.stop
	end
end

local function run_bench(filename, input, ...)
	local old_arg=arg
	local old_input=io.input()
	local title=nil
	local sclock,eclock
	for i,val in ipairs(...) do
		if run_type ~= "bench" then
			title=string.format("run script: '%s', with arg='%s'\n", filename, val)
			write(title)
		end
		io.flush()
		sclock=os.clock()
		-- load script.
		local func=loadfile(filename)
		-- create arg environment variable for script.
		local arg={}
		arg[0]=filename
		arg[1]=val
		local env=getfenv(func)
		env['arg'] = arg
		env['print'] = script_print
		-- redirect stdin
		io.input(input)
		io.flush()
		-- redirect stdout
		if output_fd ~= io.stdout then
			io.output(output_fd)
			if title then
				write(title)
			end
		end
		-- start script.
		prof_start(filename .. '_arg_' .. val .. "_profile.log", profile_mode)
		local res= {pcall(func,{val})}
		prof_stop()
--		LLVM local begin
--		eclock=os.clock()
		eclock=sclock
		print(filename);
--		LLVM local end
		io.output(io.stdout)
		-- script finished.
		io.flush()
		print("results = ", unpack(res))
		local idx=filename:find(".lua")
		write(string.format("%s:%0.2f\n", filename:sub(7, idx-1), eclock - sclock))
		collectgarbage("collect")
		io.flush()
		io.input(old_input)
	end
	arg=old_arg
end

local function print_script_list(list)
	for i,v in ipairs(list) do
		if v then
			local script,input,params = unpack(v)
			local idx=script:find(".lua")
			if script:find("bench/") == 1 then
				script = script:sub(7,idx-1)
				if input == io.stdin then input = "" end
				params = table.concat(params,",")
				write(script)
				write(":")
				write(input)
				write(":")
				write(params)
				write("\n")
			end
		end
	end
end

local scripts_full={
 {"tests/echo.lua", io.stdin, {"test","23425","908798","test"}},--only in full
 {"bench/ackermann.lua", io.stdin, {9,10,11}},
 {"bench/ary.lua", io.stdin, {3000,5000,7000,9000}},
 {"bench/binarytrees.lua", io.stdin, {12,14,16}},
 {"bench/chameneos.lua", io.stdin, {10000,100000,1000000}},
 {"bench/except.lua", io.stdin, {100000,150000,200000,250000}},
 {"bench/fannkuch.lua", io.stdin, {9,10,11}},
 {"bench/fasta.lua", io.stdin, {25000,250000,2500000}},--only in full
 {"bench/fibo.lua", io.stdin, {12,24,32}},
 {"bench/harmonic.lua", io.stdin, {6000000,8000000,10000000}},
 {"bench/hash.lua", io.stdin, {40000,60000,80000,100000}},
 {"bench/hash2.lua", io.stdin, {50,100,150,200}},
 {"bench/heapsort.lua", io.stdin, {20000,40000,60000,80000,100000}},
 {"bench/hello.lua", io.stdin, {1,50,100,150,200}},
 {"bench/knucleotide.lua", "input/knucleotide-input20000.txt", {1}},
 {"bench/lists.lua", io.stdin, {4,8,12,16,18}},
 {"bench/mandelbrot.lua", io.stdin, {200,400,600}},--only in full
 {"bench/matrix.lua", io.stdin, {10,200,400,600}},
-- {"bench/message.lua", io.stdin, {1000,2000,3000}},
 {"bench/meteor.lua", io.stdin, {0}},
 {"bench/methcall.lua", io.stdin, {100000,400000,700000,1000000}},
 {"bench/moments.lua", "input/moments-input400000.txt", {1}},
 {"bench/nbody.lua", io.stdin, {200000,2000000,20000000}},
 {"bench/nestedloop.lua", io.stdin, {8,12,16,18}},
 {"bench/nsieve.lua", io.stdin, {7,8,9}},
 {"bench/nsievebits.lua", io.stdin, {7,8,9}},
 {"bench/objinst.lua", io.stdin, {400000,700000,1000000,1500000}},
 {"bench/partialsums.lua", io.stdin, {25000,250000,2500000}},
 {"bench/pidigits.lua", io.stdin, {600,800,1000}},
 {"bench/process.lua", io.stdin, {1000,2000,3000}},
 {"bench/prodcons.lua", io.stdin, {30000,70000,100000,150000}},
 {"bench/random.lua", io.stdin, {9000,300000,600000,900000}},
 {"bench/recursive.lua", io.stdin, {3,7,11}},
 {"bench/regexdna.lua", "input/regexdna-input100000.txt", {1}},
 {"bench/regexmatch.lua", "input/regexmatch-input2000.txt", {1}},
 {"bench/revcomp.lua", "input/revcomp-input250000.txt", {1}},
 {"bench/reversefile.lua", "input/reversefile-input50.txt", {1}},
 {"bench/sieve.lua", io.stdin, {300,600,900,1200}},
 {"bench/spectralnorm.lua", io.stdin, {500,1500,2500}},
 {"bench/spellcheck.lua", "input/spellcheck-input30.txt", {1}},
 {"bench/strcat.lua", io.stdin, {10000,20000,30000,40000}},
 {"bench/sumcol.lua", "input/sumcol-input.txt", {1}},
 {"bench/takfp.lua", io.stdin, {8,9,10}},
-- {"bench/tcprequest.lua", io.stdin, {120,160,200}},
 {"bench/wc.lua", "input/wc-input3000.txt", {1}},
 {"bench/wordfreq.lua", "input/wordfreq-input20.txt", {1}}
}

local scripts_medium={
 {"bench/ackermann.lua", io.stdin, {9,10,11}},
 {"bench/ary.lua", io.stdin, {7000,8000,9000,10000}},
 {"bench/binarytrees.lua", io.stdin, {12,14,15}},
 {"bench/chameneos.lua", io.stdin, {1000000,2000000,3000000}},
 {"bench/except.lua", io.stdin, {500000,1000000,2000000,3000000}},
 {"bench/fannkuch.lua", io.stdin, {9,10,11}},
 {"bench/fibo.lua", io.stdin, {32,34,36}},
 {"bench/harmonic.lua", io.stdin, {20000000,30000000,50000000}},
 {"bench/hash.lua", io.stdin, {100000,200000,300000,400000}},
 {"bench/hash2.lua", io.stdin, {300,400,500,600}},
 {"bench/heapsort.lua", io.stdin, {200000,400000,600000,800000,1000000}},
 {"bench/hello.lua", io.stdin, {1,50,100,150,200}},
 {"bench/knucleotide.lua", "input/knucleotide-input20000.txt", {1}},
 {"bench/lists.lua", io.stdin, {20,30,40,50,60}},
 {"bench/matrix.lua", io.stdin, {200,400,600,700}},
 {"bench/meteor.lua", io.stdin, {0}},
 {"bench/methcall.lua", io.stdin, {700000,1000000,1500000,3000000}},
 {"bench/moments.lua", "input/moments-input400000.txt", {1}},
 {"bench/nbody.lua", io.stdin, {60000,200000,500000}},
 {"bench/nestedloop.lua", io.stdin, {17,18,19,20}},
 {"bench/nsieve.lua", io.stdin, {7,8,9}},
 {"bench/nsievebits.lua", io.stdin, {7,8,9}},
 {"bench/objinst.lua", io.stdin, {400000,700000,1000000,1500000}},
 {"bench/partialsums.lua", io.stdin, {250000,2500000,5000000}},
 {"bench/pidigits.lua", io.stdin, {600,800,1000}},
 {"bench/process.lua", io.stdin, {10000,20000,30000}},
 {"bench/prodcons.lua", io.stdin, {700000,800000,1000000,1500000}},
 {"bench/random.lua", io.stdin, {3000000,5000000,7000000,9000000}},
 {"bench/recursive.lua", io.stdin, {5,7,9}},
 {"bench/regexdna.lua", "input/regexdna-input100000.txt", {1}},
 {"bench/regexmatch.lua", "input/regexmatch-input2000.txt", {1}},
 {"bench/revcomp.lua", "input/revcomp-input250000.txt", {1}},
 {"bench/reversefile.lua", "input/reversefile-input50.txt", {1}},
 {"bench/sieve.lua", io.stdin, {300,600,900,1200}},
 {"bench/spectralnorm.lua", io.stdin, {500,1000}},
 {"bench/spellcheck.lua", "input/spellcheck-input30.txt", {1}},
 {"bench/strcat.lua", io.stdin, {1500000,4000000,7000000}},
 {"bench/sumcol.lua", "input/sumcol-input2000.txt", {1}},
 {"bench/takfp.lua", io.stdin, {8,9,10}},
 {"bench/wc.lua", "input/wc-input3000.txt", {1}},
 {"bench/wordfreq.lua", "input/wordfreq-input20.txt", {1}}
}

local scripts_short={
 {"bench/ackermann.lua", io.stdin, {9}},
 {"bench/ary.lua", io.stdin, {7000}},
 {"bench/binarytrees.lua", io.stdin, {12}},
 {"bench/chameneos.lua", io.stdin, {350000}},
 {"bench/except.lua", io.stdin, {500000}},
 {"bench/fannkuch.lua", io.stdin, {9}},
 {"bench/fibo.lua", io.stdin, {31}},
 {"bench/harmonic.lua", io.stdin, {20000000}},
 {"bench/hash.lua", io.stdin, {200000}},
 {"bench/hash2.lua", io.stdin, {250}},
 {"bench/heapsort.lua", io.stdin, {150000}},
 {"bench/hello.lua", io.stdin, {1}},
 {"bench/knucleotide.lua", "input/knucleotide-input20000.txt", {1}},
 {"bench/lists.lua", io.stdin, {35}},
 {"bench/matrix.lua", io.stdin, {250}},
 {"bench/meteor.lua", io.stdin, {0}},
 {"bench/methcall.lua", io.stdin, {1000000}},
 {"bench/moments.lua", "input/moments-input400000.txt", {1}},
 {"bench/nbody.lua", io.stdin, {70000}},
 {"bench/nestedloop.lua", io.stdin, {17}},
 {"bench/nsieve.lua", io.stdin, {7}},
 {"bench/nsievebits.lua", io.stdin, {6}},
 {"bench/objinst.lua", io.stdin, {400000}},
 {"bench/partialsums.lua", io.stdin, {800000}},
 {"bench/pidigits.lua", io.stdin, {150}},
 {"bench/process.lua", io.stdin, {60000}},
 {"bench/prodcons.lua", io.stdin, {1000000}},
 {"bench/random.lua", io.stdin, {3000000}},
 {"bench/recursive.lua", io.stdin, {5}},
 {"bench/regexdna.lua", "input/regexdna-input100000.txt", {1}},
 {"bench/regexmatch.lua", "input/regexmatch-input2000.txt", {1}},
 {"bench/revcomp.lua", "input/revcomp-input250000.txt", {1}},
 {"bench/reversefile.lua", "input/reversefile-input50.txt", {1}},
 {"bench/sieve.lua", io.stdin, {400}},
 {"bench/spectralnorm.lua", io.stdin, {300}},
 {"bench/spellcheck.lua", "input/spellcheck-input30.txt", {1}},
 {"bench/strcat.lua", io.stdin, {1500000}},
 {"bench/sumcol.lua", "input/sumcol-input2000.txt", {1}},
 {"bench/takfp.lua", io.stdin, {8}},
 {"bench/wc.lua", "input/wc-input3000.txt", {1}},
 {"bench/wordfreq.lua", "input/wordfreq-input20.txt", {1}}
}

-- local scripts = scripts_full
--local scripts = scripts_medium
local scripts = scripts_short
local sclock,eclock

if run_type == "bench" then
	output_fd = io.open("/dev/null","a")
	script_print = function(...) end
elseif run_type == "test" then
	if arg[2] then
		output_fd = io.open(arg[2],"a")
		script_print = redirect_print
	end
end

if run_type ~= "script_lists" then
	sclock=os.clock()
	for i,v in ipairs(scripts) do
		if v then
			run_bench(unpack(v))
		end
	end
	
	if profile_all and use_profiler then
		profiler.stop()
	end
	--LLVM local begin
	--eclock=os.clock()
	eclock=sclock
	--LLVM local end
	write(string.format("Total bench time = %f\n", eclock - sclock))
	
	io.close(output_fd)
else
	write("#!/bin/sh\n\n")
	write('SCRIPT_FULL_LIST="\n')
	print_script_list(scripts_full)
	write('"\n\n')
	write('SCRIPT_MEDIUM_LIST="\n')
	print_script_list(scripts_medium)
	write('"\n\n')
	write('SCRIPT_SHORT_LIST="\n')
	print_script_list(scripts_short)
	write('"\n\n')
end

