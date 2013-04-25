
[notes copied from wiki]

The following tools are used. They aren't all used by all configurations,
but it makes sense to describe them together. The tools are in the zip
file attached to this page.

logconv:	Converts JIT log into symbol-rich object files.
log2s:		Converts JIT log into an assembly file that can be assembled
			into a symbol rich object file by gcc.
dinkuuid:	Sets the UUID of a mach-o executable to a known value. 
			Shark uses UUIDs during symbolification to check that the object
			file containing symbols is the "same" as one it has taken sample
			data from.
binutils:	from http://macports.org

Mac AIR

	You need binutils, logconv, and dinkuuid.

	sudo port install binutils
	g++ -I/opt/local/include logconv.cpp -L/opt/local/lib -lbfd -ldl -o logconv
	g++ dinkuuid.cpp -o dinkuuid

Mac 64-bit Player

	You need binuitls, log2s, and dinkuuid.

	sudo port install binutils
	g++ log2s.cpp -o log2s
	g++ dinkuuid.cpp -o dinkuuid

Droid

	You need logconv.

	g++ logconv.cpp -lbfd -ldl -o logconv

Build the dummy shared object

	In the zip file, there is a jit.s. This small bit of assembly can be
	compiled into a shared object, which the patched player will pretend
	it is "loading" the JITted instructions from.

Mac AIR

	gcc -m32 -shared -nostartfiles -nodefaultlibs jit.s -o jit.so
	dinkuuid jit.so

Mac 64-bit Player

	gcc -shared -nostartfiles -nodefaultlibs jit.s -o jit.so
	dinkuuid jit.so

Droid

	using the arm gcc from the Android NDK
	http://developer.android.com/sdk/ndk/index.html

	arm-eabi-gcc -shared -nostartfiles -nodefaultlibs jit.s -o jit.so

