import os,sys,string

compilefile = sys.argv[1]
installfile = sys.argv[2]
srcroot = sys.argv[3]

def handleCompile(cmd):
	args = cmd.split()
	newargs = []
	skip = 0
	for arg in args:
		if skip > 0:
			skip -= 1
			continue;
		if arg == "FLASCC_SRC_DIR/build/mac/llvm-gcc-42/./gcc/xgcc":
			newargs.append("gcc")
			newargs.append("-I%s/build/mac/llvm-gcc-42/gcc" % srcroot)
			continue
		if string.find(arg, "-B") == 0:
			continue
		if string.find(arg, "--sysroot") == 0:
			continue;
		if arg == "-isystem":
			skip = 1
			continue

		arg = arg.replace("FLASCC_SRC_DIR", srcroot)
		newargs.append(arg)
	return " ".join(newargs)

f = open(compilefile)
lines = f.readlines()

objs = None
for line in lines:
	s = "ar cru .libs/libobjc-gnu.a"
	i = string.find(line, s)
	if i != -1:
		objs = line[i+len(s):]
		break
	s = "ar cru .libs/libobjc.a"
	i = string.find(line, s)
	if i != -1:
		objs = line[i+len(s):]
		break
objs = objs.split()

print "echo \"\" > tmp-runtime.m"
for line in lines:
	if string.find(line, "runtime-info.h") != -1 and string.find(line, "xgcc") != -1:
		print handleCompile(line)
		break

for obj in objs:
	for line in lines:
		i1 = string.find(line, obj)
		i2 = string.find(line, "xgcc")

		if i1 != -1 and i2 != -1:
			print handleCompile(line)

print "ar crus libobjc.a %s" % (" ".join(objs))

f = open(installfile)
lines = f.readlines()
headers = []
for line in lines:
	s = "for file in"
	i1 = string.find(line, s)
	if i1 != -1:
		headers = line[i1+len(s):string.find(line,";")].split()
		break

print "mkdir -p %s/sdk/usr/include/objc" % (srcroot)
print "cp -f libobjc.a %s/sdk/usr/lib/" % (srcroot)
for h in headers:
	print "cp -f %s/llvm-gcc-4.2-2.9/libobjc/objc/%s %s/sdk/usr/include/objc/" % (srcroot, h, srcroot)
