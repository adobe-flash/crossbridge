/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html )
    

	Not all compilers support <cstdint> yet, and some don't even have <stdint.h>
	So, we have to do our own type definitions.
*/


/******************************************************************************/

#if _MSC_VER

	// for platforms that don't include the C99 header stdint.h
	
	// if your platform does not include stdint.h, please edit the #if above
	// to include your platform (and remember that it has to work elsewhere as well)

	template<bool P, typename T, typename E>
	struct if_;

	template<typename T, typename E>
	struct if_<true, T, E> { typedef T type; };

	template<typename T, typename E>
	struct if_<false, T, E>{ typedef E type; };

	typedef signed char int8_t;
	typedef unsigned char uint8_t;

	typedef signed short int16_t;
	typedef unsigned short uint16_t;

	typedef if_<sizeof(int) == 4, int, long>::type int32_t;
	typedef if_<sizeof(unsigned int) == 4, unsigned int, unsigned long>::type uint32_t;

	#if _MSC_VER
		typedef __int64 int64_t;
		typedef unsigned __int64 uint64_t;
	#else
		typedef long long int64_t;
		typedef unsigned long long uint64_t;
	#endif

#else

	// for platforms that do have stdint.h
	
	// if your platform fails to find this header, please edit the #if above
	// to include your platform (and remember that it has to work elsewhere as well)
	
	#include <stdint.h>
	
#endif

/******************************************************************************/
