/* APPLE LOCAL file radar 4668465 */
/* Test if addition of 'volatile' to object causes bogus error in presence of try-catch. */
/* { dg-options "-fpreprocessed -Wno-long-long" } */
/* { dg-do compile } */

# 1 "VolatileBug.mm"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "VolatileBug.mm"
# 1 "/System/Library/Frameworks/Foundation.framework/Headers/NSArray.h" 1 3




# 1 "/System/Library/Frameworks/Foundation.framework/Headers/NSObject.h" 1 3




# 1 "/System/Library/Frameworks/Foundation.framework/Headers/NSObjCRuntime.h" 1 3
# 50 "/System/Library/Frameworks/Foundation.framework/Headers/NSObjCRuntime.h" 3
# 1 "/usr/include/objc/objc.h" 1 3 4
# 31 "/usr/include/objc/objc.h" 3 4
# 1 "/usr/include/objc/objc-api.h" 1 3 4
# 28 "/usr/include/objc/objc-api.h" 3 4
# 1 "/usr/include/AvailabilityMacros.h" 1 3 4
# 29 "/usr/include/objc/objc-api.h" 2 3 4
# 32 "/usr/include/objc/objc.h" 2 3 4
# 1 "/usr/include/sys/types.h" 1 3 4
# 67 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/sys/appleapiopts.h" 1 3 4
# 68 "/usr/include/sys/types.h" 2 3 4


# 1 "/usr/include/sys/cdefs.h" 1 3 4
# 71 "/usr/include/sys/types.h" 2 3 4


# 1 "/usr/include/machine/types.h" 1 3 4
# 32 "/usr/include/machine/types.h" 3 4
# 1 "/usr/include/i386/types.h" 1 3 4
# 65 "/usr/include/i386/types.h" 3 4
# 1 "/usr/include/i386/_types.h" 1 3 4
# 32 "/usr/include/i386/_types.h" 3 4
typedef signed char __int8_t;



typedef unsigned char __uint8_t;
typedef short __int16_t;
typedef unsigned short __uint16_t;
typedef int __int32_t;
typedef unsigned int __uint32_t;
typedef long long __int64_t;
typedef unsigned long long __uint64_t;

typedef long __darwin_intptr_t;
typedef unsigned int __darwin_natural_t;
# 65 "/usr/include/i386/_types.h" 3 4
typedef int __darwin_ct_rune_t;





typedef union {
 char __mbstate8[128];
 long long _mbstateL;
} __mbstate_t;

typedef __mbstate_t __darwin_mbstate_t;


typedef int __darwin_ptrdiff_t;





typedef long unsigned int __darwin_size_t;





typedef __builtin_va_list __darwin_va_list;





typedef int __darwin_wchar_t;




typedef __darwin_wchar_t __darwin_rune_t;


typedef int __darwin_wint_t;




typedef unsigned long __darwin_clock_t;
typedef __uint32_t __darwin_socklen_t;
typedef long __darwin_ssize_t;
typedef long __darwin_time_t;
# 66 "/usr/include/i386/types.h" 2 3 4







typedef signed char int8_t;

typedef unsigned char u_int8_t;


typedef short int16_t;

typedef unsigned short u_int16_t;


typedef int int32_t;

typedef unsigned int u_int32_t;


typedef long long int64_t;

typedef unsigned long long u_int64_t;




typedef int32_t register_t;




typedef __darwin_intptr_t intptr_t;



typedef unsigned long int uintptr_t;






typedef u_int64_t user_addr_t;
typedef u_int64_t user_size_t;
typedef int64_t user_ssize_t;
typedef int64_t user_long_t;
typedef u_int64_t user_ulong_t;
typedef int64_t user_time_t;





typedef u_int64_t syscall_arg_t;
# 33 "/usr/include/machine/types.h" 2 3 4
# 74 "/usr/include/sys/types.h" 2 3 4
# 1 "/usr/include/sys/_types.h" 1 3 4
# 28 "/usr/include/sys/_types.h" 3 4
# 1 "/usr/include/machine/_types.h" 1 3 4
# 29 "/usr/include/sys/_types.h" 2 3 4
# 53 "/usr/include/sys/_types.h" 3 4
struct __darwin_pthread_handler_rec
{
 void (*__routine)(void *);
 void *__arg;
 struct __darwin_pthread_handler_rec *__next;
};
struct _opaque_pthread_attr_t { long __sig; char __opaque[36]; };
struct _opaque_pthread_cond_t { long __sig; char __opaque[24]; };
struct _opaque_pthread_condattr_t { long __sig; char __opaque[4]; };
struct _opaque_pthread_mutex_t { long __sig; char __opaque[40]; };
struct _opaque_pthread_mutexattr_t { long __sig; char __opaque[8]; };
struct _opaque_pthread_once_t { long __sig; char __opaque[4]; };
struct _opaque_pthread_rwlock_t { long __sig; char __opaque[124]; };
struct _opaque_pthread_rwlockattr_t { long __sig; char __opaque[12]; };
struct _opaque_pthread_t { long __sig; struct __darwin_pthread_handler_rec *__cleanup_stack; char __opaque[596]; };
# 89 "/usr/include/sys/_types.h" 3 4
typedef __int64_t __darwin_blkcnt_t;
typedef __int32_t __darwin_blksize_t;
typedef __int32_t __darwin_dev_t;
typedef unsigned int __darwin_fsblkcnt_t;
typedef unsigned int __darwin_fsfilcnt_t;
typedef __uint32_t __darwin_gid_t;
typedef __uint32_t __darwin_id_t;
typedef __uint32_t __darwin_ino_t;
typedef __int64_t __darwin_ino64_t;
typedef __darwin_natural_t __darwin_mach_port_name_t;
typedef __darwin_mach_port_name_t __darwin_mach_port_t;
typedef __uint16_t __darwin_mode_t;
typedef __int64_t __darwin_off_t;
typedef __int32_t __darwin_pid_t;
typedef struct _opaque_pthread_attr_t
   __darwin_pthread_attr_t;
typedef struct _opaque_pthread_cond_t
   __darwin_pthread_cond_t;
typedef struct _opaque_pthread_condattr_t
   __darwin_pthread_condattr_t;
typedef unsigned long __darwin_pthread_key_t;
typedef struct _opaque_pthread_mutex_t
   __darwin_pthread_mutex_t;
typedef struct _opaque_pthread_mutexattr_t
   __darwin_pthread_mutexattr_t;
typedef struct _opaque_pthread_once_t
   __darwin_pthread_once_t;
typedef struct _opaque_pthread_rwlock_t
   __darwin_pthread_rwlock_t;
typedef struct _opaque_pthread_rwlockattr_t
   __darwin_pthread_rwlockattr_t;
typedef struct _opaque_pthread_t
   *__darwin_pthread_t;
typedef __uint32_t __darwin_sigset_t;
typedef __int32_t __darwin_suseconds_t;
typedef __uint32_t __darwin_uid_t;
typedef __uint32_t __darwin_useconds_t;
typedef unsigned char __darwin_uuid_t[16];
# 75 "/usr/include/sys/types.h" 2 3 4

# 1 "/usr/include/machine/endian.h" 1 3 4
# 32 "/usr/include/machine/endian.h" 3 4
# 1 "/usr/include/i386/endian.h" 1 3 4
# 94 "/usr/include/i386/endian.h" 3 4
# 1 "/usr/include/sys/_endian.h" 1 3 4
# 114 "/usr/include/sys/_endian.h" 3 4
# 1 "/usr/include/libkern/_OSByteOrder.h" 1 3 4
# 61 "/usr/include/libkern/_OSByteOrder.h" 3 4
# 1 "/usr/include/libkern/i386/_OSByteOrder.h" 1 3 4
# 39 "/usr/include/libkern/i386/_OSByteOrder.h" 3 4
static inline
__uint16_t
_OSSwapInt16(
    __uint16_t _data
)
{
    return ((_data << 8) | (_data >> 8));
}

static inline
__uint32_t
_OSSwapInt32(
    __uint32_t _data
)
{
    __asm__ ("bswap   %0" : "+r" (_data));
    return _data;
}


static inline
__uint64_t
_OSSwapInt64(
    __uint64_t _data
)
{
    __asm__ ("bswap   %%eax\n\t"
             "bswap   %%edx\n\t"
             "xchgl   %%eax, %%edx"
             : "+A" (_data));
    return _data;
}
# 62 "/usr/include/libkern/_OSByteOrder.h" 2 3 4
# 115 "/usr/include/sys/_endian.h" 2 3 4
# 95 "/usr/include/i386/endian.h" 2 3 4
# 33 "/usr/include/machine/endian.h" 2 3 4
# 77 "/usr/include/sys/types.h" 2 3 4


typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;

typedef unsigned long u_long;


typedef unsigned short ushort;
typedef unsigned int uint;


typedef u_int64_t u_quad_t;
typedef int64_t quad_t;
typedef quad_t * qaddr_t;

typedef char * caddr_t;
typedef int32_t daddr_t;


typedef __darwin_dev_t dev_t;



typedef u_int32_t fixpt_t;


typedef __darwin_blkcnt_t blkcnt_t;




typedef __darwin_blksize_t blksize_t;




typedef __darwin_gid_t gid_t;





typedef __uint32_t in_addr_t;




typedef __uint16_t in_port_t;



typedef __darwin_ino_t ino_t;





typedef __darwin_ino64_t ino64_t;






typedef __int32_t key_t;



typedef __darwin_mode_t mode_t;




typedef __uint16_t nlink_t;





typedef __darwin_id_t id_t;



typedef __darwin_pid_t pid_t;




typedef __darwin_off_t off_t;



typedef int32_t segsz_t;
typedef int32_t swblk_t;


typedef __darwin_uid_t uid_t;
# 192 "/usr/include/sys/types.h" 3 4
static inline __int32_t major(__uint32_t _x)
{
 return (__int32_t)(((__uint32_t)_x >> 24) & 0xff);
}

static inline __int32_t minor(__uint32_t _x)
{
 return (__int32_t)((_x) & 0xffffff);
}

static inline dev_t makedev(__uint32_t _major, __uint32_t _minor)
{
 return (dev_t)(((_major) << 24) | (_minor));
}
# 218 "/usr/include/sys/types.h" 3 4
typedef __darwin_clock_t clock_t;






typedef __darwin_size_t size_t;




typedef __darwin_ssize_t ssize_t;




typedef __darwin_time_t time_t;




typedef __darwin_useconds_t useconds_t;




typedef __darwin_suseconds_t suseconds_t;
# 255 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/sys/_structs.h" 1 3 4
# 193 "/usr/include/sys/_structs.h" 3 4
extern "C" {
typedef struct fd_set {
 __int32_t fds_bits[(((1024) + (((sizeof(__int32_t) * 8)) - 1)) / ((sizeof(__int32_t) * 8)))];
} fd_set;
}


static inline int
__darwin_fd_isset(int _n, struct fd_set *_p)
{
 return (_p->fds_bits[_n/(sizeof(__int32_t) * 8)] & (1<<(_n % (sizeof(__int32_t) * 8))));
}
# 256 "/usr/include/sys/types.h" 2 3 4




typedef __int32_t fd_mask;
# 313 "/usr/include/sys/types.h" 3 4
typedef __darwin_pthread_attr_t pthread_attr_t;



typedef __darwin_pthread_cond_t pthread_cond_t;



typedef __darwin_pthread_condattr_t pthread_condattr_t;



typedef __darwin_pthread_mutex_t pthread_mutex_t;



typedef __darwin_pthread_mutexattr_t pthread_mutexattr_t;



typedef __darwin_pthread_once_t pthread_once_t;



typedef __darwin_pthread_rwlock_t pthread_rwlock_t;



typedef __darwin_pthread_rwlockattr_t pthread_rwlockattr_t;



typedef __darwin_pthread_t pthread_t;






typedef __darwin_pthread_key_t pthread_key_t;





typedef __darwin_fsblkcnt_t fsblkcnt_t;




typedef __darwin_fsfilcnt_t fsfilcnt_t;
# 33 "/usr/include/objc/objc.h" 2 3 4


typedef struct objc_class *Class;
typedef struct objc_object {
    Class isa;
} *id;


typedef struct objc_selector *SEL;
typedef id (*IMP)(id, SEL, ...);
typedef signed char BOOL;
# 73 "/usr/include/objc/objc.h" 3 4
    typedef int arith_t;
    typedef unsigned uarith_t;



extern "C" BOOL sel_isMapped(SEL sel);
extern "C" const char *sel_getName(SEL sel);
extern "C" SEL sel_getUid(const char *str);
extern "C" SEL sel_registerName(const char *str);
extern "C" const char *object_getClassName(id obj);
extern "C" void *object_getIndexedIvars(id obj);

typedef char *STR;
# 51 "/System/Library/Frameworks/Foundation.framework/Headers/NSObjCRuntime.h" 2 3
# 1 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/stdarg.h" 1 3 4
# 43 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 105 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/stdarg.h" 3 4
typedef __gnuc_va_list va_list;
# 52 "/System/Library/Frameworks/Foundation.framework/Headers/NSObjCRuntime.h" 2 3
# 1 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/stdint.h" 1 3 4
# 34 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/stdint.h" 3 4
typedef unsigned char uint8_t;




typedef unsigned short uint16_t;




typedef unsigned int uint32_t;




typedef unsigned long long uint64_t;



typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;
typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;



typedef int8_t int_fast8_t;
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;
typedef uint8_t uint_fast8_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;
# 91 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/stdint.h" 3 4
typedef long long int intmax_t;
# 100 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/stdint.h" 3 4
typedef long long unsigned int uintmax_t;
# 53 "/System/Library/Frameworks/Foundation.framework/Headers/NSObjCRuntime.h" 2 3
# 1 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/limits.h" 1 3 4
# 11 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/limits.h" 3 4
# 1 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/syslimits.h" 1 3 4






# 1 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/limits.h" 1 3 4
# 122 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/limits.h" 3 4
# 1 "/usr/include/limits.h" 1 3 4
# 64 "/usr/include/limits.h" 3 4
# 1 "/usr/include/machine/limits.h" 1 3 4







# 1 "/usr/include/i386/limits.h" 1 3 4
# 40 "/usr/include/i386/limits.h" 3 4
# 1 "/usr/include/i386/_limits.h" 1 3 4
# 41 "/usr/include/i386/limits.h" 2 3 4
# 9 "/usr/include/machine/limits.h" 2 3 4
# 65 "/usr/include/limits.h" 2 3 4
# 1 "/usr/include/sys/syslimits.h" 1 3 4
# 66 "/usr/include/limits.h" 2 3 4
# 123 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/limits.h" 2 3 4
# 8 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/syslimits.h" 2 3 4
# 12 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/limits.h" 2 3 4
# 54 "/System/Library/Frameworks/Foundation.framework/Headers/NSObjCRuntime.h" 2 3


extern "C" double NSFoundationVersionNumber;
# 97 "/System/Library/Frameworks/Foundation.framework/Headers/NSObjCRuntime.h" 3
typedef int NSInt;
typedef unsigned int NSUInt;
typedef int NSInteger;
typedef unsigned int NSUInteger;
# 109 "/System/Library/Frameworks/Foundation.framework/Headers/NSObjCRuntime.h" 3
@class NSString;

extern "C" NSString *NSStringFromSelector(SEL aSelector);
extern "C" SEL NSSelectorFromString(NSString *aSelectorName);
extern "C" Class NSClassFromString(NSString *aClassName);
extern "C" NSString *NSStringFromClass(Class aClass);
extern "C" const char *NSGetSizeAndAlignment(const char *typePtr, NSUInteger *sizep, NSUInteger *alignp);

extern "C" void NSLog(NSString *format, ...);
extern "C" void NSLogv(NSString *format, va_list args);

enum _NSComparisonResult {NSOrderedAscending = -1, NSOrderedSame, NSOrderedDescending};
typedef NSInteger NSComparisonResult;

enum {NSNotFound = 2147483647L};
# 6 "/System/Library/Frameworks/Foundation.framework/Headers/NSObject.h" 2 3
# 1 "/System/Library/Frameworks/Foundation.framework/Headers/NSZone.h" 1 3





# 1 "/System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h" 1 3
# 39 "/System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h" 3
# 1 "/usr/lib/gcc/i686-apple-darwin8/4.0.1/include/stdbool.h" 1 3 4
# 40 "/System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h" 2 3
# 49 "/System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h" 3
# 1 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 1 3
# 20 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
# 1 "/System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Headers/ConditionalMacros.h" 1 3
# 42 "/System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Headers/ConditionalMacros.h" 3
# 1 "/usr/include/TargetConditionals.h" 1 3 4
# 43 "/System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Headers/ConditionalMacros.h" 2 3
# 21 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 2 3
# 33 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
extern "C" {


#pragma pack(push, 2)
# 126 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef unsigned char UInt8;
typedef signed char SInt8;
typedef unsigned short UInt16;
typedef signed short SInt16;





typedef unsigned long UInt32;
typedef signed long SInt32;
# 153 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
struct wide {
  UInt32 lo;
  SInt32 hi;
};
typedef struct wide wide;
struct UnsignedWide {
  UInt32 lo;
  UInt32 hi;
};
typedef struct UnsignedWide UnsignedWide;
# 184 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
      typedef signed long long SInt64;
        typedef unsigned long long UInt64;
# 204 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef SInt32 Fixed;
typedef Fixed * FixedPtr;
typedef SInt32 Fract;
typedef Fract * FractPtr;
typedef UInt32 UnsignedFixed;
typedef UnsignedFixed * UnsignedFixedPtr;
typedef short ShortFixed;
typedef ShortFixed * ShortFixedPtr;
# 231 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef float Float32;
typedef double Float64;
struct Float80 {
    SInt16 exp;
    UInt16 man[4];
};
typedef struct Float80 Float80;

struct Float96 {
    SInt16 exp[2];
    UInt16 man[4];
};
typedef struct Float96 Float96;
struct Float32Point {
    Float32 x;
    Float32 y;
};
typedef struct Float32Point Float32Point;
# 259 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef char * Ptr;
typedef Ptr * Handle;
typedef long Size;
# 289 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef SInt16 OSErr;
typedef SInt32 OSStatus;
typedef void * LogicalAddress;
typedef const void * ConstLogicalAddress;
typedef void * PhysicalAddress;
typedef UInt8 * BytePtr;
typedef unsigned long ByteCount;
typedef unsigned long ByteOffset;
typedef SInt32 Duration;
typedef UnsignedWide AbsoluteTime;
typedef UInt32 OptionBits;
typedef unsigned long ItemCount;
typedef UInt32 PBVersion;
typedef SInt16 ScriptCode;
typedef SInt16 LangCode;
typedef SInt16 RegionCode;
typedef UInt32 FourCharCode;
typedef FourCharCode OSType;
typedef FourCharCode ResType;
typedef OSType * OSTypePtr;
typedef ResType * ResTypePtr;
# 320 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef unsigned char Boolean;
# 333 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef long ( * ProcPtr)();
typedef void ( * Register68kProcPtr)();




typedef ProcPtr UniversalProcPtr;


typedef ProcPtr * ProcHandle;
typedef UniversalProcPtr * UniversalProcHandle;
# 358 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef void * PRefCon;




typedef UInt32 URefCon;
typedef SInt32 SRefCon;
# 388 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
enum {
  noErr = 0
};

enum {
  kNilOptions = 0
};


enum {
  kVariableLengthArray = 1
};

enum {
  kUnknownType = 0x3F3F3F3F
};
# 457 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef UInt32 UnicodeScalarValue;
typedef UInt32 UTF32Char;
typedef UInt16 UniChar;
typedef UInt16 UTF16Char;
typedef UInt8 UTF8Char;
typedef UniChar * UniCharPtr;
typedef unsigned long UniCharCount;
typedef UniCharCount * UniCharCountPtr;
typedef unsigned char Str255[256];
typedef unsigned char Str63[64];
typedef unsigned char Str32[33];
typedef unsigned char Str31[32];
typedef unsigned char Str27[28];
typedef unsigned char Str15[16];
# 479 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef unsigned char Str32Field[34];
# 489 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef Str63 StrFileName;
typedef unsigned char * StringPtr;
typedef StringPtr * StringHandle;
typedef const unsigned char * ConstStringPtr;
typedef const unsigned char * ConstStr255Param;
typedef const unsigned char * ConstStr63Param;
typedef const unsigned char * ConstStr32Param;
typedef const unsigned char * ConstStr31Param;
typedef const unsigned char * ConstStr27Param;
typedef const unsigned char * ConstStr15Param;
typedef ConstStr63Param ConstStrFileNameParam;

inline unsigned char StrLength(ConstStr255Param string) { return (*string); }
# 516 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
struct ProcessSerialNumber {
  UInt32 highLongOfPSN;
  UInt32 lowLongOfPSN;
};
typedef struct ProcessSerialNumber ProcessSerialNumber;
typedef ProcessSerialNumber * ProcessSerialNumberPtr;
# 538 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
struct Point {
  short v;
  short h;
};
typedef struct Point Point;
typedef Point * PointPtr;
struct Rect {
  short top;
  short left;
  short bottom;
  short right;
};
typedef struct Rect Rect;
typedef Rect * RectPtr;
struct FixedPoint {
  Fixed x;
  Fixed y;
};
typedef struct FixedPoint FixedPoint;
struct FixedRect {
  Fixed left;
  Fixed top;
  Fixed right;
  Fixed bottom;
};
typedef struct FixedRect FixedRect;

typedef short CharParameter;
enum {
  normal = 0,
  bold = 1,
  italic = 2,
  underline = 4,
  outline = 8,
  shadow = 0x10,
  condense = 0x20,
  extend = 0x40
};

typedef unsigned char Style;
typedef short StyleParameter;
typedef Style StyleField;
# 594 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
typedef SInt32 TimeValue;
typedef SInt32 TimeScale;
typedef wide CompTimeValue;
typedef SInt64 TimeValue64;
typedef struct TimeBaseRecord* TimeBase;
struct TimeRecord {
  CompTimeValue value;
  TimeScale scale;
  TimeBase base;
};
typedef struct TimeRecord TimeRecord;
# 646 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
struct NumVersion {

  UInt8 nonRelRev;
  UInt8 stage;
  UInt8 minorAndBugRev;
  UInt8 majorRev;
};
typedef struct NumVersion NumVersion;


enum {

  developStage = 0x20,
  alphaStage = 0x40,
  betaStage = 0x60,
  finalStage = 0x80
};

union NumVersionVariant {

  NumVersion parts;
  UInt32 whole;
};
typedef union NumVersionVariant NumVersionVariant;
typedef NumVersionVariant * NumVersionVariantPtr;
typedef NumVersionVariantPtr * NumVersionVariantHandle;
struct VersRec {

  NumVersion numericVersion;
  short countryCode;
  Str255 shortVersion;
  Str255 reserved;
};
typedef struct VersRec VersRec;
typedef VersRec * VersRecPtr;
typedef VersRecPtr * VersRecHndl;





typedef UInt8 Byte;
typedef SInt8 SignedByte;
typedef wide * WidePtr;
typedef UnsignedWide * UnsignedWidePtr;
typedef Float80 extended80;
typedef Float96 extended96;
typedef SInt8 VHSelect;
# 707 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
extern void
Debugger(void) ;
# 719 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
extern void
DebugStr(ConstStr255Param debuggerMsg) ;
# 766 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
extern void
SysBreak(void) ;
# 778 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
extern void
SysBreakStr(ConstStr255Param debuggerMsg) ;
# 790 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
extern void
SysBreakFunc(ConstStr255Param debuggerMsg) ;
# 801 "/System/Library/Frameworks/CoreServices.framework/Headers/../Frameworks/CarbonCore.framework/Headers/MacTypes.h" 3
#pragma pack(pop)


}
# 50 "/System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h" 2 3
# 91 "/System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h" 3
extern "C" {
# 154 "/System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h" 3
extern double kCFCoreFoundationVersionNumber;
# 190 "/System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h" 3
typedef unsigned long CFTypeID;
typedef unsigned long CFOptionFlags;
typedef unsigned long CFHashCode;
typedef signed long CFIndex;


typedef const void * CFTypeRef;

typedef const struct __CFString * CFStringRef;
typedef struct __CFString * CFMutableStringRef;






typedef CFTypeRef CFPropertyListRef;


enum {
    kCFCompareLessThan = -1,
    kCFCompareEqualTo = 0,
    kCFCompareGreaterThan = 1
};
typedef CFIndex CFComparisonResult;


typedef CFComparisonResult (*CFComparatorFunction)(const void *val1, const void *val2, void *context);



enum {
    kCFNotFound = -1
};



typedef struct {
    CFIndex location;
    CFIndex length;
} CFRange;


static __inline__ __attribute__((always_inline)) CFRange CFRangeMake(CFIndex loc, CFIndex len) {
    CFRange range;
    range.location = loc;
    range.length = len;
    return range;
}





extern
CFRange __CFRangeMake(CFIndex loc, CFIndex len);





typedef const struct __CFNull * CFNullRef;

extern
CFTypeID CFNullGetTypeID(void);

extern
const CFNullRef kCFNull;
# 271 "/System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h" 3
typedef const struct __CFAllocator * CFAllocatorRef;


extern
const CFAllocatorRef kCFAllocatorDefault;


extern
const CFAllocatorRef kCFAllocatorSystemDefault;







extern
const CFAllocatorRef kCFAllocatorMalloc;





extern
const CFAllocatorRef kCFAllocatorMallocZone ;





extern
const CFAllocatorRef kCFAllocatorNull;





extern
const CFAllocatorRef kCFAllocatorUseContext;

typedef const void * (*CFAllocatorRetainCallBack)(const void *info);
typedef void (*CFAllocatorReleaseCallBack)(const void *info);
typedef CFStringRef (*CFAllocatorCopyDescriptionCallBack)(const void *info);
typedef void * (*CFAllocatorAllocateCallBack)(CFIndex allocSize, CFOptionFlags hint, void *info);
typedef void * (*CFAllocatorReallocateCallBack)(void *ptr, CFIndex newsize, CFOptionFlags hint, void *info);
typedef void (*CFAllocatorDeallocateCallBack)(void *ptr, void *info);
typedef CFIndex (*CFAllocatorPreferredSizeCallBack)(CFIndex size, CFOptionFlags hint, void *info);
typedef struct {
    CFIndex version;
    void * info;
    CFAllocatorRetainCallBack retain;
    CFAllocatorReleaseCallBack release;
    CFAllocatorCopyDescriptionCallBack copyDescription;
    CFAllocatorAllocateCallBack allocate;
    CFAllocatorReallocateCallBack reallocate;
    CFAllocatorDeallocateCallBack deallocate;
    CFAllocatorPreferredSizeCallBack preferredSize;
} CFAllocatorContext;

extern
CFTypeID CFAllocatorGetTypeID(void);
# 356 "/System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h" 3
extern
void CFAllocatorSetDefault(CFAllocatorRef allocator);

extern
CFAllocatorRef CFAllocatorGetDefault(void);

extern
CFAllocatorRef CFAllocatorCreate(CFAllocatorRef allocator, CFAllocatorContext *context);

extern
void *CFAllocatorAllocate(CFAllocatorRef allocator, CFIndex size, CFOptionFlags hint);

extern
void *CFAllocatorReallocate(CFAllocatorRef allocator, void *ptr, CFIndex newsize, CFOptionFlags hint);

extern
void CFAllocatorDeallocate(CFAllocatorRef allocator, void *ptr);

extern
CFIndex CFAllocatorGetPreferredSizeForSize(CFAllocatorRef allocator, CFIndex size, CFOptionFlags hint);

extern
void CFAllocatorGetContext(CFAllocatorRef allocator, CFAllocatorContext *context);




extern
CFTypeID CFGetTypeID(CFTypeRef cf);

extern
CFStringRef CFCopyTypeIDDescription(CFTypeID type_id);

extern
CFTypeRef CFRetain(CFTypeRef cf);

extern
void CFRelease(CFTypeRef cf);

extern
CFIndex CFGetRetainCount(CFTypeRef cf);

extern
CFTypeRef CFMakeCollectable(CFTypeRef cf) ;

extern
Boolean CFEqual(CFTypeRef cf1, CFTypeRef cf2);

extern
CFHashCode CFHash(CFTypeRef cf);

extern
CFStringRef CFCopyDescription(CFTypeRef cf);

extern
CFAllocatorRef CFGetAllocator(CFTypeRef cf);


}
# 7 "/System/Library/Frameworks/Foundation.framework/Headers/NSZone.h" 2 3

@class NSString;

typedef struct _NSZone NSZone;


extern "C" NSZone *NSDefaultMallocZone(void);
extern "C" NSZone *NSCreateZone(NSUInteger startSize, NSUInteger granularity, BOOL canFree);
extern "C" void NSRecycleZone(NSZone *zone);
extern "C" void NSSetZoneName(NSZone *zone, NSString *name);
extern "C" NSString *NSZoneName(NSZone *zone);
extern "C" NSZone *NSZoneFromPointer(void *ptr);

extern "C" void *NSZoneMalloc(NSZone *zone, NSUInteger size);
extern "C" void *NSZoneCalloc(NSZone *zone, NSUInteger numElems, NSUInteger byteSize);
extern "C" void *NSZoneRealloc(NSZone *zone, void *ptr, NSUInteger size);
extern "C" void NSZoneFree(NSZone *zone, void *ptr);
# 32 "/System/Library/Frameworks/Foundation.framework/Headers/NSZone.h" 3
enum {
    NSScannedOption = (1<<0),
    NSCollectorDisabledOption = (1<<1),
};


extern "C" void * NSAllocateCollectable(NSUInteger size, NSUInteger options) ;
extern "C" void * NSReallocateCollectable(void *ptr, NSUInteger size, NSUInteger options) ;
# 48 "/System/Library/Frameworks/Foundation.framework/Headers/NSZone.h" 3
static __inline__ __attribute__((always_inline)) id NSMakeCollectable(CFTypeRef cf) {
    return (id) (cf != __null ? CFMakeCollectable(cf) : __null);
}


extern "C" NSUInteger NSPageSize(void);
extern "C" NSUInteger NSLogPageSize(void);
extern "C" NSUInteger NSRoundUpToMultipleOfPageSize(NSUInteger bytes);
extern "C" NSUInteger NSRoundDownToMultipleOfPageSize(NSUInteger bytes);
extern "C" void *NSAllocateMemoryPages(NSUInteger bytes);
extern "C" void NSDeallocateMemoryPages(void *ptr, NSUInteger bytes);
extern "C" void NSCopyMemoryPages(const void *source, void *dest, NSUInteger bytes);
extern "C" NSUInteger NSRealMemoryAvailable(void);
# 7 "/System/Library/Frameworks/Foundation.framework/Headers/NSObject.h" 2 3

@class NSInvocation, NSMethodSignature, NSCoder, NSString, NSEnumerator;
@class Protocol;



@protocol NSObject

- (BOOL)isEqual:(id)object;
- (NSUInteger)hash;

- (Class)superclass;
- (Class)class;
- (id)self;
- (NSZone *)zone;

- (id)performSelector:(SEL)aSelector;
- (id)performSelector:(SEL)aSelector withObject:(id)object;
- (id)performSelector:(SEL)aSelector withObject:(id)object1 withObject:(id)object2;

- (BOOL)isProxy;

- (BOOL)isKindOfClass:(Class)aClass;
- (BOOL)isMemberOfClass:(Class)aClass;
- (BOOL)conformsToProtocol:(Protocol *)aProtocol;

- (BOOL)respondsToSelector:(SEL)aSelector;

- (id)retain;
- (oneway void)release;
- (id)autorelease;
- (NSUInteger)retainCount;

- (NSString *)description;

@end

@protocol NSCopying

- (id)copyWithZone:(NSZone *)zone;

@end

@protocol NSMutableCopying

- (id)mutableCopyWithZone:(NSZone *)zone;

@end

@protocol NSCoding

- (void)encodeWithCoder:(NSCoder *)aCoder;
- (id)initWithCoder:(NSCoder *)aDecoder;

@end



@interface NSObject <NSObject> {
    Class isa;
}

+ (void)load;

+ (void)initialize;
- (id)init;

+ (id)new;
+ (id)allocWithZone:(NSZone *)zone;
+ (id)alloc;
- (void)dealloc;


- (void)finalize;


- (id)copy;
- (id)mutableCopy;

+ (id)copyWithZone:(NSZone *)zone;
+ (id)mutableCopyWithZone:(NSZone *)zone;

+ (Class)superclass;
+ (Class)class;
+ (void)poseAsClass:(Class)aClass;
+ (BOOL)instancesRespondToSelector:(SEL)aSelector;
+ (BOOL)conformsToProtocol:(Protocol *)protocol;
- (IMP)methodForSelector:(SEL)aSelector;
+ (IMP)instanceMethodForSelector:(SEL)aSelector;
+ (NSInteger)version;
+ (void)setVersion:(NSInteger)aVersion;
- (void)doesNotRecognizeSelector:(SEL)aSelector;
- (void)forwardInvocation:(NSInvocation *)anInvocation;
- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector;

+ (NSMethodSignature *)instanceMethodSignatureForSelector:(SEL)aSelector;


+ (BOOL)isSubclassOfClass:(Class)aClass;


+ (NSString *)description;

- (Class)classForCoder;
- (id)replacementObjectForCoder:(NSCoder *)aCoder;
- (id)awakeAfterUsingCoder:(NSCoder *)aDecoder;

@end



extern "C" id NSAllocateObject(Class aClass, NSUInteger extraBytes, NSZone *zone);

extern "C" void NSDeallocateObject(id object);

extern "C" id NSCopyObject(id object, NSUInteger extraBytes, NSZone *zone);

extern "C" BOOL NSShouldRetainWithZone(id anObject, NSZone *requestedZone);

extern "C" void NSIncrementExtraRefCount(id object);

extern "C" BOOL NSDecrementExtraRefCountWasZero(id object);

extern "C" NSUInteger NSExtraRefCount(id object);
# 6 "/System/Library/Frameworks/Foundation.framework/Headers/NSArray.h" 2 3
# 1 "/System/Library/Frameworks/Foundation.framework/Headers/NSRange.h" 1 3




# 1 "/System/Library/Frameworks/Foundation.framework/Headers/NSValue.h" 1 3






@class NSString, NSDictionary;

@interface NSValue : NSObject <NSCopying, NSCoding>

- (void)getValue:(void *)value;
- (const char *)objCType;

@end

@interface NSValue (NSValueCreation)

- (id)initWithBytes:(const void *)value objCType:(const char *)type;
+ (NSValue *)valueWithBytes:(const void *)value objCType:(const char *)type;
+ (NSValue *)value:(const void *)value withObjCType:(const char *)type;

@end

@interface NSValue (NSValueExtensionMethods)

+ (NSValue *)valueWithNonretainedObject:(id)anObject;
- (id)nonretainedObjectValue;

+ (NSValue *)valueWithPointer:(const void *)pointer;
- (void *)pointerValue;

- (BOOL)isEqualToValue:(NSValue *)value;

@end

@interface NSNumber : NSValue

- (char)charValue;
- (unsigned char)unsignedCharValue;
- (short)shortValue;
- (unsigned short)unsignedShortValue;
- (int)intValue;
- (unsigned int)unsignedIntValue;
- (long)longValue;
- (unsigned long)unsignedLongValue;
- (long long)longLongValue;
- (unsigned long long)unsignedLongLongValue;
- (float)floatValue;
- (double)doubleValue;
- (BOOL)boolValue;

- (NSInteger)integerValue;
- (NSUInteger)unsignedIntegerValue;


- (NSString *)stringValue;

- (NSComparisonResult)compare:(NSNumber *)otherNumber;

- (BOOL)isEqualToNumber:(NSNumber *)number;

- (NSString *)descriptionWithLocale:(NSDictionary *)locale;

@end

@interface NSNumber (NSNumberCreation)

- (id)initWithChar:(char)value;
- (id)initWithUnsignedChar:(unsigned char)value;
- (id)initWithShort:(short)value;
- (id)initWithUnsignedShort:(unsigned short)value;
- (id)initWithInt:(int)value;
- (id)initWithUnsignedInt:(unsigned int)value;
- (id)initWithLong:(long)value;
- (id)initWithUnsignedLong:(unsigned long)value;
- (id)initWithLongLong:(long long)value;
- (id)initWithUnsignedLongLong:(unsigned long long)value;
- (id)initWithFloat:(float)value;
- (id)initWithDouble:(double)value;
- (id)initWithBool:(BOOL)value;

- (id)initWithInteger:(NSInteger)value;
- (id)initWithUnsignedInteger:(NSUInteger)value;


+ (NSNumber *)numberWithChar:(char)value;
+ (NSNumber *)numberWithUnsignedChar:(unsigned char)value;
+ (NSNumber *)numberWithShort:(short)value;
+ (NSNumber *)numberWithUnsignedShort:(unsigned short)value;
+ (NSNumber *)numberWithInt:(int)value;
+ (NSNumber *)numberWithUnsignedInt:(unsigned int)value;
+ (NSNumber *)numberWithLong:(long)value;
+ (NSNumber *)numberWithUnsignedLong:(unsigned long)value;
+ (NSNumber *)numberWithLongLong:(long long)value;
+ (NSNumber *)numberWithUnsignedLongLong:(unsigned long long)value;
+ (NSNumber *)numberWithFloat:(float)value;
+ (NSNumber *)numberWithDouble:(double)value;
+ (NSNumber *)numberWithBool:(BOOL)value;

+ (NSNumber *)numberWithInteger:(NSInteger)value;
+ (NSNumber *)numberWithUnsignedInteger:(NSUInteger)value;


@end
# 6 "/System/Library/Frameworks/Foundation.framework/Headers/NSRange.h" 2 3


@class NSString;

typedef struct _NSRange {
    NSUInteger location;
    NSUInteger length;
} NSRange;

typedef NSRange *NSRangePointer;

static __inline__ __attribute__((always_inline)) NSRange NSMakeRange(NSUInteger loc, NSUInteger len) {
    NSRange r;
    r.location = loc;
    r.length = len;
    return r;
}

static __inline__ __attribute__((always_inline)) NSUInteger NSMaxRange(NSRange range) {
    return (range.location + range.length);
}

static __inline__ __attribute__((always_inline)) BOOL NSLocationInRange(NSUInteger loc, NSRange range) {
    return (loc - range.location < range.length);
}

static __inline__ __attribute__((always_inline)) BOOL NSEqualRanges(NSRange range1, NSRange range2) {
    return (range1.location == range2.location && range1.length == range2.length);
}

extern "C" NSRange NSUnionRange(NSRange range1, NSRange range2);
extern "C" NSRange NSIntersectionRange(NSRange range1, NSRange range2);
extern "C" NSString *NSStringFromRange(NSRange range);
extern "C" NSRange NSRangeFromString(NSString *aString);

@interface NSValue (NSValueRangeExtensions)

+ (NSValue *)valueWithRange:(NSRange)range;
- (NSRange)rangeValue;

@end
# 7 "/System/Library/Frameworks/Foundation.framework/Headers/NSArray.h" 2 3

@class NSData, NSDictionary, NSEnumerator, NSIndexSet, NSString, NSURL;
@protocol NSFastEnumeration;



@interface NSArray : NSObject <NSCopying, NSMutableCopying, NSCoding, NSFastEnumeration>

- (NSUInteger)count;
- (id)objectAtIndex:(NSUInteger)index;

@end

@interface NSArray (NSExtendedArray)

- (NSArray *)arrayByAddingObject:(id)anObject;
- (NSArray *)arrayByAddingObjectsFromArray:(NSArray *)otherArray;
- (NSString *)componentsJoinedByString:(NSString *)separator;
- (BOOL)containsObject:(id)anObject;
- (NSString *)description;
- (NSString *)descriptionWithLocale:(NSDictionary *)locale;
- (NSString *)descriptionWithLocale:(NSDictionary *)locale indent:(NSUInteger)level;
- (id)firstObjectCommonWithArray:(NSArray *)otherArray;
- (void)getObjects:(id *)objects;
- (void)getObjects:(id *)objects range:(NSRange)range;
- (NSUInteger)indexOfObject:(id)anObject;
- (NSUInteger)indexOfObject:(id)anObject inRange:(NSRange)range;
- (NSUInteger)indexOfObjectIdenticalTo:(id)anObject;
- (NSUInteger)indexOfObjectIdenticalTo:(id)anObject inRange:(NSRange)range;
- (BOOL)isEqualToArray:(NSArray *)otherArray;
- (id)lastObject;
- (NSEnumerator *)objectEnumerator;
- (NSEnumerator *)reverseObjectEnumerator;
- (NSData *)sortedArrayHint;
- (NSArray *)sortedArrayUsingFunction:(NSInteger (*)(id, id, void *))comparator context:(void *)context;
- (NSArray *)sortedArrayUsingFunction:(NSInteger (*)(id, id, void *))comparator context:(void *)context hint:(NSData *)hint;
- (NSArray *)sortedArrayUsingSelector:(SEL)comparator;
- (NSArray *)subarrayWithRange:(NSRange)range;
- (BOOL)writeToFile:(NSString *)path atomically:(BOOL)useAuxiliaryFile;
- (BOOL)writeToURL:(NSURL *)url atomically:(BOOL)atomically;

- (void)makeObjectsPerformSelector:(SEL)aSelector;
- (void)makeObjectsPerformSelector:(SEL)aSelector withObject:(id)argument;


- (NSArray *)objectsAtIndexes:(NSIndexSet *)indexes;


@end

@interface NSArray (NSArrayCreation)

+ (id)array;
+ (id)arrayWithContentsOfFile:(NSString *)path;
+ (id)arrayWithContentsOfURL:(NSURL *)url;
+ (id)arrayWithObject:(id)anObject;
+ (id)arrayWithObjects:(id)firstObj, ...;
- (id)initWithArray:(NSArray *)array;

- (id)initWithArray:(NSArray *)array copyItems:(BOOL)flag;

- (id)initWithContentsOfFile:(NSString *)path;
- (id)initWithContentsOfURL:(NSURL *)url;
- (id)initWithObjects:(id *)objects count:(NSUInteger)count;
- (id)initWithObjects:(id)firstObj, ...;

+ (id)arrayWithArray:(NSArray *)array;
+ (id)arrayWithObjects:(id *)objs count:(NSUInteger)cnt;

@end



@interface NSMutableArray : NSArray

- (void)addObject:(id)anObject;
- (void)insertObject:(id)anObject atIndex:(NSUInteger)index;
- (void)removeLastObject;
- (void)removeObjectAtIndex:(NSUInteger)index;
- (void)replaceObjectAtIndex:(NSUInteger)index withObject:(id)anObject;

@end

@interface NSMutableArray (NSExtendedMutableArray)

- (void)addObjectsFromArray:(NSArray *)otherArray;
- (void)exchangeObjectAtIndex:(NSUInteger)idx1 withObjectAtIndex:(NSUInteger)idx2;
- (void)removeAllObjects;
- (void)removeObject:(id)anObject inRange:(NSRange)range;
- (void)removeObject:(id)anObject;
- (void)removeObjectIdenticalTo:(id)anObject inRange:(NSRange)range;
- (void)removeObjectIdenticalTo:(id)anObject;
- (void)removeObjectsFromIndices:(NSUInteger *)indices numIndices:(NSUInteger)count;
- (void)removeObjectsInArray:(NSArray *)otherArray;
- (void)removeObjectsInRange:(NSRange)range;
- (void)replaceObjectsInRange:(NSRange)range withObjectsFromArray:(NSArray *)otherArray range:(NSRange)otherRange;
- (void)replaceObjectsInRange:(NSRange)range withObjectsFromArray:(NSArray *)otherArray;
- (void)setArray:(NSArray *)otherArray;
- (void)sortUsingFunction:(NSInteger (*)(id, id, void *))compare context:(void *)context;
- (void)sortUsingSelector:(SEL)comparator;


- (void)insertObjects:(NSArray *)objects atIndexes:(NSIndexSet *)indexes;
- (void)removeObjectsAtIndexes:(NSIndexSet *)indexes;
- (void)replaceObjectsAtIndexes:(NSIndexSet *)indexes withObjects:(NSArray *)objects;


@end

@interface NSMutableArray (NSMutableArrayCreation)

+ (id)arrayWithCapacity:(NSUInteger)numItems;
- (id)initWithCapacity:(NSUInteger)numItems;

@end
# 2 "VolatileBug.mm" 2
# 1 "/System/Library/Frameworks/Foundation.framework/Headers/NSException.h" 1 3






# 1 "/usr/include/setjmp.h" 1 3 4
# 26 "/usr/include/setjmp.h" 3 4
# 1 "/usr/include/machine/setjmp.h" 1 3 4
# 32 "/usr/include/machine/setjmp.h" 3 4
# 1 "/usr/include/i386/setjmp.h" 1 3 4
# 64 "/usr/include/i386/setjmp.h" 3 4
typedef int jmp_buf[(18)];
typedef int sigjmp_buf[(18) + 1];




extern "C" {
int setjmp(jmp_buf);
void longjmp(jmp_buf, int);


int _setjmp(jmp_buf);
void _longjmp(jmp_buf, int);
int sigsetjmp(sigjmp_buf, int);
void siglongjmp(sigjmp_buf, int);



void longjmperror(void);

}
# 33 "/usr/include/machine/setjmp.h" 2 3 4
# 27 "/usr/include/setjmp.h" 2 3 4
# 8 "/System/Library/Frameworks/Foundation.framework/Headers/NSException.h" 2 3

@class NSString, NSDictionary;



extern "C" NSString * const NSGenericException;
extern "C" NSString * const NSRangeException;
extern "C" NSString * const NSInvalidArgumentException;
extern "C" NSString * const NSInternalInconsistencyException;

extern "C" NSString * const NSMallocException;

extern "C" NSString * const NSObjectInaccessibleException;
extern "C" NSString * const NSObjectNotAvailableException;
extern "C" NSString * const NSDestinationInvalidException;

extern "C" NSString * const NSPortTimeoutException;
extern "C" NSString * const NSInvalidSendPortException;
extern "C" NSString * const NSInvalidReceivePortException;
extern "C" NSString * const NSPortSendException;
extern "C" NSString * const NSPortReceiveException;

extern "C" NSString * const NSOldStyleException;



@interface NSException : NSObject <NSCopying, NSCoding> {
    @private
    NSString *name;
    NSString *reason;
    NSDictionary *userInfo;
    void *reserved;
}

+ (NSException *)exceptionWithName:(NSString *)name reason:(NSString *)reason userInfo:(NSDictionary *)userInfo;
- (id)initWithName:(NSString *)aName reason:(NSString *)aReason userInfo:(NSDictionary *)aUserInfo;

- (NSString *)name;
- (NSString *)reason;
- (NSDictionary *)userInfo;

- (void)raise;

@end

@interface NSException (NSExceptionRaisingConveniences)

+ (void)raise:(NSString *)name format:(NSString *)format, ...;
+ (void)raise:(NSString *)name format:(NSString *)format arguments:(va_list)argList;

@end
# 68 "/System/Library/Frameworks/Foundation.framework/Headers/NSException.h" 3
typedef void NSUncaughtExceptionHandler(NSException *exception);

extern "C" NSUncaughtExceptionHandler *NSGetUncaughtExceptionHandler(void);
extern "C" void NSSetUncaughtExceptionHandler(NSUncaughtExceptionHandler *);

@class NSAssertionHandler;
# 154 "/System/Library/Frameworks/Foundation.framework/Headers/NSException.h" 3
@interface NSAssertionHandler : NSObject {
    @private
    void *_reserved;
}

+ (NSAssertionHandler *)currentHandler;

- (void)handleFailureInMethod:(SEL)selector object:(id)object file:(NSString *)fileName lineNumber:(NSInteger)line description:(NSString *)format,...;

- (void)handleFailureInFunction:(NSString *)functionName file:(NSString *)fileName lineNumber:(NSInteger)line description:(NSString *)format,...;

@end
# 3 "VolatileBug.mm" 2
# 1 "/usr/include/stdio.h" 1 3 4
# 64 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/_types.h" 1 3 4
# 29 "/usr/include/_types.h" 3 4
typedef int __darwin_nl_item;
typedef int __darwin_wctrans_t;



typedef unsigned long __darwin_wctype_t;
# 65 "/usr/include/stdio.h" 2 3 4
# 87 "/usr/include/stdio.h" 3 4
typedef __darwin_off_t fpos_t;
# 98 "/usr/include/stdio.h" 3 4
struct __sbuf {
 unsigned char *_base;
 int _size;
};


struct __sFILEX;
# 132 "/usr/include/stdio.h" 3 4
typedef struct __sFILE {
 unsigned char *_p;
 int _r;
 int _w;
 short _flags;
 short _file;
 struct __sbuf _bf;
 int _lbfsize;


 void *_cookie;
 int (*_close)(void *);
 int (*_read) (void *, char *, int);
 fpos_t (*_seek) (void *, fpos_t, int);
 int (*_write)(void *, const char *, int);


 struct __sbuf _ub;
 struct __sFILEX *_extra;
 int _ur;


 unsigned char _ubuf[3];
 unsigned char _nbuf[1];


 struct __sbuf _lb;


 int _blksize;
 fpos_t _offset;
} FILE;

extern "C" {

extern FILE *__stdinp;
extern FILE *__stdoutp;
extern FILE *__stderrp;



}
# 248 "/usr/include/stdio.h" 3 4
extern "C" {
void clearerr(FILE *);
int fclose(FILE *);
int feof(FILE *);
int ferror(FILE *);
int fflush(FILE *);
int fgetc(FILE *);
int fgetpos(FILE * , fpos_t *);
char *fgets(char * , int, FILE *);
FILE *fopen(const char * , const char * );
int fprintf(FILE * , const char * , ...) ;
int fputc(int, FILE *);
int fputs(const char * , FILE * ) __asm("_" "fputs" "$UNIX2003");
size_t fread(void * , size_t, size_t, FILE * );
FILE *freopen(const char * , const char * ,
     FILE * ) __asm("_" "freopen" "$UNIX2003");
int fscanf(FILE * , const char * , ...) ;
int fseek(FILE *, long, int);
int fsetpos(FILE *, const fpos_t *);
long ftell(FILE *);
size_t fwrite(const void * , size_t, size_t, FILE * ) __asm("_" "fwrite" "$UNIX2003");
int getc(FILE *);
int getchar(void);
char *gets(char *);

extern const int sys_nerr;
extern const char *const sys_errlist[];

void perror(const char *);
int printf(const char * , ...) ;
int putc(int, FILE *);
int putchar(int);
int puts(const char *);
int remove(const char *);
int rename (const char *, const char *);
void rewind(FILE *);
int scanf(const char * , ...) ;
void setbuf(FILE * , char * );
int setvbuf(FILE * , char * , int, size_t);
int sprintf(char * , const char * , ...) ;
int sscanf(const char * , const char * , ...) ;
FILE *tmpfile(void);
char *tmpnam(char *);
int ungetc(int, FILE *);
int vfprintf(FILE * , const char * , va_list) ;
int vprintf(const char * , va_list) ;
int vsprintf(char * , const char * , va_list) ;

int asprintf(char **, const char *, ...) ;
int vasprintf(char **, const char *, va_list) ;

}







extern "C" {
char *ctermid(char *);

char *ctermid_r(char *);

FILE *fdopen(int, const char *);

char *fgetln(FILE *, size_t *);

int fileno(FILE *);
void flockfile(FILE *);

const char
 *fmtcheck(const char *, const char *);
int fpurge(FILE *);

int fseeko(FILE *, off_t, int);
off_t ftello(FILE *);
int ftrylockfile(FILE *);
void funlockfile(FILE *);
int getc_unlocked(FILE *);
int getchar_unlocked(void);

int getw(FILE *);

int pclose(FILE *);
FILE *popen(const char *, const char *);
int putc_unlocked(int, FILE *);
int putchar_unlocked(int);

int putw(int, FILE *);
void setbuffer(FILE *, char *, int);
int setlinebuf(FILE *);

int snprintf(char * , size_t, const char * , ...) ;
char *tempnam(const char *, const char *) __asm("_" "tempnam" "$UNIX2003");
int vfscanf(FILE * , const char * , va_list) ;
int vscanf(const char * , va_list) ;
int vsnprintf(char * , size_t, const char * , va_list) ;
int vsscanf(const char * , const char * , va_list) ;

FILE *zopen(const char *, const char *, int);

}





extern "C" {
FILE *funopen(const void *,
  int (*)(void *, char *, int),
  int (*)(void *, const char *, int),
  fpos_t (*)(void *, fpos_t, int),
  int (*)(void *));
}
# 371 "/usr/include/stdio.h" 3 4
extern "C" {
int __srget(FILE *);
int __svfscanf(FILE *, const char *, va_list) ;
int __swbuf(int, FILE *);
}







static inline int __sputc(int _c, FILE *_p) {
 if (--_p->_w >= 0 || (_p->_w >= _p->_lbfsize && (char)_c != '\n'))
  return (*_p->_p++ = _c);
 else
  return (__swbuf(_c, _p));
}
# 4 "VolatileBug.mm" 2

class CSharedObject {
 public:
  CSharedObject* Retain();
  void Release();
  CSharedObject& operator=(const CSharedObject& other);
  long CountReferences();
  static void LogAllocCount();
 protected:
  CSharedObject();
  CSharedObject(const CSharedObject& other);
  virtual ~CSharedObject();
 private:
  int32_t mRefCount;
  uint32_t mSignature;
 public:
  static void LogNullRef();
};
# 34 "VolatileBug.mm"
template <class T> class CSharedObjectRef {
 public:
  ~CSharedObjectRef()
  {
   if (mObjectPtr != __null) {
    mObjectPtr->Release();
    mObjectPtr = __null;
   }
  };
  CSharedObjectRef() : mObjectPtr(__null)
  {
  };




  CSharedObjectRef(const CSharedObjectRef& other) : mObjectPtr(other.mObjectPtr)
  {
   if ( mObjectPtr )
    mObjectPtr->Retain();
  };



  explicit CSharedObjectRef(T* other) : mObjectPtr(other)
  {
   if ( mObjectPtr )
    mObjectPtr->Retain();
  };




  CSharedObjectRef& operator=(const CSharedObjectRef& other)
  {
   if (other.mObjectPtr != mObjectPtr) {



    if (mObjectPtr != __null )
     mObjectPtr->Release();


    mObjectPtr = (T*)other.mObjectPtr;

    if ( mObjectPtr != __null )
     mObjectPtr->Retain();

   }

   return *this;
  }




  CSharedObjectRef& operator=(const T* other)
  {
   if (other != mObjectPtr) {
    if (mObjectPtr != __null )
     mObjectPtr->Release();

    mObjectPtr = const_cast<T*>(other);

    if (mObjectPtr != __null )
     mObjectPtr->Retain();
   }
   return *this;
  }



  T& operator*() const
  {
   if (mObjectPtr == __null) {
    CSharedObject::LogNullRef();
   }

   return *mObjectPtr;
  };

  T* operator->() const
  {
   if (mObjectPtr == __null) {
    CSharedObject::LogNullRef();
   }

   return mObjectPtr;
  };

  bool operator==(const CSharedObjectRef& other) const
  {
   return mObjectPtr == other.mObjectPtr;
  }

  bool operator==(const T* other) const
  {
   return mObjectPtr == other;
  }

  bool operator!=(const CSharedObjectRef& other) const
  {
   return mObjectPtr != other.mObjectPtr;
  }

  bool operator!=(const T* other) const
  {
   return mObjectPtr != other;
  }

  bool operator!() const
  {
   return !mObjectPtr;
  }
# 160 "VolatileBug.mm"
  template <class subT> subT* dynamicCast()
  {
   subT* result = dynamic_cast<subT*>(mObjectPtr);
   return result;
  }

  const T* getObjectPtr() const
  {
   return mObjectPtr;
  }

 protected:
  T* mObjectPtr;
};
# 190 "VolatileBug.mm"
class MyClass : public CSharedObject
{
public:
 MyClass ();
 virtual ~MyClass ();
 virtual void setValue (float f);
};

class MyClassRef : public CSharedObjectRef<MyClass> { protected: public: MyClassRef() {} MyClassRef( MyClass* assignPtr ) : CSharedObjectRef<MyClass>(assignPtr) {} MyClassRef( const MyClassRef& ref ) : CSharedObjectRef<MyClass>(ref) {} MyClassRef& operator=( MyClass* assignPtr ) { CSharedObjectRef<MyClass>::operator=(assignPtr); return *this; } };

@interface VolatileBug : NSObject
- (void) mymethod: (MyClassRef) myobject withParameters: (NSMutableArray *) parameters;
@end

@implementation VolatileBug
- (void) mymethod: (MyClassRef) myobject withParameters: (NSMutableArray *) parameterList;
{
 NSNumber *parameter = [NSNumber numberWithDouble: 0];

 @try {
  [parameterList addObject: parameter];
 } @catch (NSException *localException) {
  printf("could not add parameter to list\n");
 }

 myobject->setValue(0);
}

@end
