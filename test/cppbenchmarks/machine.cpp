/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)


The purpose of this source file is to report information about the compiler,
OS and machine running the benchmark

When adding reporting for your compiler, OS and CPU:
	Please remember that this source file has to compile everywhere else as well.

See http://predef.sourceforge.net/precomp.html for some older compilers
and architectures.
See source for Unix hostinfo.

All trademarks used herein are the property of their owner, and are only used
for correct identification of their products

*/

/******************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include "benchmark_stdint.hpp"

// this should be defined on Mach derived OSes (MacOS, FreeBSD, etc.)
#if defined(_MACHTYPES_H_)
#include <sys/sysctl.h>
#endif


/******************************************************************************/

void VerifyTypeSizes()
{
	if (sizeof(int8_t) != 1)
		printf("Found size of int8_t was %d instead of 1\n", (int)sizeof(int8_t) );
	if (sizeof(uint8_t) != 1)
		printf("Found size of uint8_t was %d instead of 1\n", (int)sizeof(uint8_t) );
	if (sizeof(int16_t) != 2)
		printf("Found size of int16_t was %d instead of 2\n", (int)sizeof(int16_t) );
	if (sizeof(uint16_t) != 2)
		printf("Found size of uint16_t was %d instead of 2\n", (int)sizeof(uint16_t) );
	if (sizeof(int32_t) != 4)
		printf("Found size of int32_t was %d instead of 4\n", (int)sizeof(int32_t) );
	if (sizeof(uint32_t) != 4)
		printf("Found size of uint32_t was %d instead of 4\n", (int)sizeof(uint32_t) );
	if (sizeof(int64_t) != 8)
		printf("Found size of int64_t was %d instead of 8\n", (int)sizeof(int64_t) );
	if (sizeof(uint64_t) != 8)
		printf("Found size of uint64_t was %d instead of 8\n", (int)sizeof(uint64_t) );
}

/******************************************************************************/

// compiler version and any major targetting options (such as 32 vs 64 bit)
void ReportCompiler() 
{

	printf("##Compiler\n");

#if __INTEL_COMPILER

	printf("Intel Compiler version %d\n", __INTEL_COMPILER );
	printf("Build %d\n", __INTEL_COMPILER_BUILD_DATE );

	#if defined(__WIN32__) || defined(_WIN32)
		printf("Compiling for Windows 32 bit\n" );
	#endif
	
	#if __WIN64__
		printf("Compiling for Windows 64 bit\n" );
	#endif

#elif _MSC_VER

	printf("Microsoft VisualC++ version %d\n", _MSC_VER );
	
	#if defined(__WIN32__) || defined(_WIN32)
		printf("Compiling for Windows 32 bit\n" );
	#endif
	
	#if __WIN64__
		printf("Compiling for Windows 64 bit\n" );
	#endif
	
	#if defined(__CLR_VER)
		printf("CLR version %s\n", __CLR_VER );
	#endif

	/*
	See http://msdn2.microsoft.com/en-us/library/b0084kay(VS.80).aspx#_predir_table_1..3
	*/

#elif __GNUC__

	printf("GCC version %s\n", __VERSION__ );
	
	/*
	printf("%d.%d", __GNUC__, __GNUC_MINOR__ );
	#if defined(__GNUC_PATCHLEVEL__)
		printf(" . %d", __GNUC_PATCHLEVEL__ );
	#endif
	printf("\n");
	*/
	
	#if __LP64__
		printf("Compiled for LP64\n");
	#endif
	
	
	/*
	Other useful macros:
	__OPTIMIZE__
	__OPTIMIZE_SIZE__
	__NO_INLINE__

	See http://developer.apple.com/documentation/DeveloperTools/gcc-4.0.1/cpp/Common-Predefined-Macros.html
	*/
	
#elif __MWERKS__
	printf("Metrowerks CodeWarrior version 0x%8.8lX\n", __MWERKS__ );
#elif __MRC__
	printf("Apple MrC[pp] version 0x%8.8lX\n", __MRC__ );
#elif __MOTO__
	printf("Motorola MCC version 0x%8.8lX\n", __MOTO__ );
#else
	printf("********\n" );
	printf("Unknown compiler, please update %s for your compiler\n", __FILE__ );
	printf("********\n" );
#endif

}

/******************************************************************************/

// these seem to be available on most current Mach derived OSes
// TODO - ccox - disabled because some Linux distros have incomplete sets of headers
#if 0 && defined(_MACHTYPES_H_)
// if the headers are not present, these includes will break
// then the macro test above and the report below need to be updated
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#endif

#if defined(_MSC_VER)
#include <hash_map>
#include <hash_set>
#endif


// STL and optional bits
void ReportHeaders() 
{

#if defined(_MACHTYPES_H_)

	printf("Includes preliminary TR1 unordered_map\n");
	printf("Includes preliminary TR1 unordered_set\n");
	printf("Includes preliminary TR1 unordered_multimap\n");
	printf("Includes preliminary TR1 unordered_multiset\n");

#endif

#if defined(_MSC_VER)
	printf("Includes preliminary TR1 hash_set\n");
	printf("Includes preliminary TR1 hash_map\n");
#endif

}

/******************************************************************************/

// what kind of CPU is the compiler targetting?
void ReportCPUTarget()
{

	printf("##Target CPU\n");

#if _MANAGED

	printf("Compiled for Microsoft managed code (CLR)\n" );

#elif defined(__ppc64__)

	printf("Compiled for PowerPC 64bit\n");

#elif defined(__powerc) || defined(__ppc__) || defined(powerpc) || defined(ppc)

	printf("Compiled for PowerPC 32bit\n");

#elif defined(_M_IA64)

	printf("Compiled for Intel Architecture 64\n" );

#elif defined(_M_X64) || defined(__x86_64__)

	printf("Compiled for x86 64bit\n" );

#elif defined(__i386__) || defined(i386) || defined(_X86_) || defined(_M_IX86)

	printf("Compiled for x86 32bit\n" );

	#if _M_IX86
	switch( _M_IX86) {
	case 300:
		printf("Compiled for 80386\n" );
		break;
	case 400:
		printf("Compiled for 80486\n" );
		break;
	case 500:
		printf("Compiled for Pentium\n" );
		break;
	case 600:
		printf("Compiled for PentiumII\n" );
		break;
	default:	
		printf("********\n" );
		printf("Unknown x86 target, please update %s for your cpu\n", __FILE__ );
		printf("********\n" );
		break;
	}
	#endif

#elif defined(_ALPHA_)

	printf("Compiled for Alpha\n" );
	
#else
	printf("********\n" );
	printf("Unknown target CPU, please update %s for your cpu\n", __FILE__ );
	printf("********\n" );
#endif

}

/******************************************************************************/

// byte order of the CPU we're running on
void ReportEndian()
{
	static uint32_t cookie = 0x01020304;
	unsigned char *testPtr = (unsigned char *) &cookie;
	
	if (*testPtr == 0x01) {
		printf( "Big Endian\n" );
	} else if (*testPtr == 0x04) {
		printf( "Little Endian\n" );
	} else {
		printf("********\n" );
		printf("Unknown byteorder, please update %s for your cpu\n", __FILE__ );
		printf("********\n" );
	}
}

/******************************************************************************/

	
// what CPU are we actually running on
// architecture, revision, speed

// methods for obtaining this information are probably OS specific
	
void ReportCPUPhysical()
{
	const int one_million = 1000000L;
	
	printf("##Physical CPU\n");



// this should work for any Mach based OS (MacOS, FreeBSD, etc.)
#if defined(_MACHTYPES_H_)

// see sysctl.h for the definitions
	{
	long returnBuffer=0, retval=0;
	long long bigBuffer = 0;
	size_t len;
	
	// this gets us the CPU family, but not the exact CPU model and rev!
	len = 4;
	retval = sysctlbyname("hw.cputype", &returnBuffer, &len, NULL, 0);
	if (retval == 0) {
		printf("Mach CPU type %ld\n", returnBuffer );
	
		// from sys/machine.h
		switch(returnBuffer) {
			case 1:
				printf("CPU_TYPE VAX\n");
				break;
			case 6:
				printf("CPU_TYPE MC680x0\n");
				break;
			case 7:
				printf("CPU_TYPE x86\n");
				break;
			case 8:
				printf("CPU_TYPE MIPS\n");
				break;
			case 10:
				printf("CPU_TYPE MC98000\n");
				break;
			case 11:
				printf("CPU_TYPE HPPA\n");
				break;
			case 12:
				printf("CPU_TYPE ARM\n");
				break;
			case 13:
				printf("CPU_TYPE MC8880x0\n");
				break;
			case 14:
				printf("CPU_TYPE SPARC\n");
				break;
			case 15:
				printf("CPU_TYPE i860\n");
				break;
			case 16:
				printf("CPU_TYPE Alpha\n");
				break;
			case 18:
				printf("CPU_TYPE PowerPC\n");
				break;
			default:
				printf("********\n" );
				printf("Unknown Mach CPU Type, please update %s for your cpu\n", __FILE__ );
				printf("********\n" );
				break;
		}
	
	}
	
	// corresponds to CPU types, but the list is kinda big and dependent on CPU major type
	len = 4;
	retval = sysctlbyname("hw.cpusubtype", &returnBuffer, &len, NULL, 0);
	if (retval == 0)
		printf("Mach CPU subtype %ld\n", returnBuffer );
	
	len = 8;
	retval = sysctlbyname("hw.cpufrequency_max", &bigBuffer, &len, NULL, 0);
	if (retval == 0)
		printf("CPU frequency: %.2f Mhz\n", (double)bigBuffer/one_million );
	
	
	// PowerPC CPU extensions
	len = 4;
	retval = sysctlbyname("hw.optional.floatingpoint", &returnBuffer, &len, NULL, 0);
	if (retval == 0 && returnBuffer != 0)
		printf("CPU has optional floating point instructions\n" );
	
	len = 4;
	retval = sysctlbyname("hw.optional.altivec", &returnBuffer, &len, NULL, 0);
	if (retval == 0 && returnBuffer != 0)
		printf("CPU has AltiVec instructions\n" );
	
	len = 4;
	retval = sysctlbyname("hw.optional.64bitops", &returnBuffer, &len, NULL, 0);
	if (retval == 0 && returnBuffer != 0)
		printf("CPU has 64 bit instructions\n" );
	
	len = 4;
	retval = sysctlbyname("hw.optional.fsqrt", &returnBuffer, &len, NULL, 0);
	if (retval == 0 && returnBuffer != 0)
		printf("CPU has fsqrt instruction\n" );
	
	
	// x86 CPU extension
	len = 4;
	retval = sysctlbyname("hw.optional.mmx", &returnBuffer, &len, NULL, 0);
	if (retval == 0 && returnBuffer != 0)
		printf("CPU has MMX instructions\n" );
	
	len = 4;
	retval = sysctlbyname("hw.optional.sse", &returnBuffer, &len, NULL, 0);
	if (retval == 0 && returnBuffer != 0)
		printf("CPU has SSE instructions\n" );
	
	len = 4;
	retval = sysctlbyname("hw.optional.sse2", &returnBuffer, &len, NULL, 0);
	if (retval == 0 && returnBuffer != 0)
		printf("CPU has SSE2 instructions\n" );
	
	len = 4;
	retval = sysctlbyname("hw.optional.sse3", &returnBuffer, &len, NULL, 0);
	if (retval == 0 && returnBuffer != 0)
		printf("CPU has SSE3 instructions\n" );
	
	len = 4;
	retval = sysctlbyname("hw.optional.sse4", &returnBuffer, &len, NULL, 0);
	if (retval == 0 && returnBuffer != 0)
		printf("CPU has SSE4 instructions\n" );
	
	len = 4;
	retval = sysctlbyname("hw.optional.sse5", &returnBuffer, &len, NULL, 0);
	if (retval == 0 && returnBuffer != 0)
		printf("CPU has SSE5 instructions\n" );
	
	len = 4;
	retval = sysctlbyname("hw.optional.x86_64", &returnBuffer, &len, NULL, 0);
	if (retval == 0 && returnBuffer != 0)
		printf("CPU has x86_64 instructions\n" );
	
	}
	
#endif	// _MACHTYPES_H_
	
	// useful information, and not so dependent
	ReportEndian();
}

/******************************************************************************/

// format a number of bytes and print (without return)
void printMemSize( long long input )
{
	double meg = (double)input / (1024.0*1024.0);
	double tera = (double)input / (1024.0*1024.0*1024.0*1024.0);

	if (input < 1024) {	// format as bytes
		printf("%lld bytes", input );
	} else if (input < (1024*1024)) {	// format as KB
		printf("%.2f KBytes", (double)input/1024.0 );
	} else if (meg < 1024.0) {	// format as MB
		printf("%.2f MBytes", meg );
	} else if (meg < (1024.0*1024.0)) {	// format as GB
		printf("%.2f GBytes", meg/1024.0 );
	} else if (tera < (1024.0)) {	// format as TB
		printf("%.2f TeraBytes", tera );
	} else if (tera < (1024.0*1024.0)) {	// format as PB
		printf("%.2f PetaBytes", tera/1024.0 );
	} else {	// format as EB
		printf("%.2f ExaBytes", tera/(1024.0*1024.0) );
	}
}

/******************************************************************************/

// information about the machine, outside of the CPU
void ReportMachinePhysical()
{
	printf("##Machine\n");
	

// this should work for any Mach based OS (MacOS, FreeBSD, etc.)
#if defined(_MACHTYPES_H_)

// see sysctl.h for the definitions
	{
	long returnBuffer=0, retval=0;
	long long bigBuffer = 0;
	size_t len;
	
	len = 4;
	returnBuffer = 0;
	retval = sysctlbyname("hw.ncpu", &returnBuffer, &len, NULL, 0);
	if (retval == 0) {
		printf("Machine has %ld CPUs\n", returnBuffer );
		}

	
	len = 4;
	retval = sysctlbyname("hw.physicalcpu_max", &returnBuffer, &len, NULL, 0);
	if (retval == 0)
		printf("Machine has %ld physical CPUs\n", returnBuffer );
	
	len = 4;
	retval = sysctlbyname("hw.logicalcpu_max", &returnBuffer, &len, NULL, 0);
	if (retval == 0)
		printf("Machine has %ld logical CPUs\n", returnBuffer );
	
	
	len = 8;
	retval = sysctlbyname("hw.memsize", &bigBuffer, &len, NULL, 0);
	if (retval == 0) {
		printf("Machine has ");
		printMemSize( bigBuffer );
		printf(" of RAM\n");
		}
	
	len = 8;
	retval = sysctlbyname("hw.pagesize", &bigBuffer, &len, NULL, 0);
	if (retval == 0) {
		printf("Machine using ");
		printMemSize( bigBuffer );
		printf(" pagesize\n");
		}

	}
	
#endif	// _MACHTYPES_H_

	
}

/******************************************************************************/

void ReportOS()
{
	printf("##Operating System\n");
	

// this should work for any Mach based OS (MacOS, FreeBSD, etc.)
#if defined(_MACHTYPES_H_)

// see sysctl.h for the definitions
	{
	//long returnBuffer;
	//long long bigBuffer;
	char string_buffer[1024];
	long retval=0;
	int mib[4];
	size_t len;
	
	mib[0] = CTL_KERN;
	mib[1] = KERN_VERSION;
	len = sizeof(string_buffer);
	retval = sysctl(mib, 2, string_buffer, &len, NULL, 0);
	if (retval == 0)
		printf("Kernel OS Version: %s\n", string_buffer );
	
	}
	
#endif	// _MACHTYPES_H_
}

/******************************************************************************/

int main (int argc, char *argv[])
{
	// this should only be changed when the reporting tags have changed in an incompatible way
	const char version[] = "version 1.0";

	printf("##Start machine report %s\n", version );
	VerifyTypeSizes();
	ReportCompiler();
	ReportCPUTarget();
	ReportCPUPhysical();
	ReportMachinePhysical();
	ReportOS();
	printf("##End machine report\n");

	return 0;
}

