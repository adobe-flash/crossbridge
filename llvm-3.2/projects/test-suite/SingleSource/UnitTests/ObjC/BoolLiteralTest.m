#import <Foundation/Foundation.h>

int main(int argc, char *argv[])
{
#ifdef __clang__
  @autoreleasepool {
    if (CFGetTypeID((id)@(__objc_yes)) == CFBooleanGetTypeID()) {
      ;
    } else {
     abort();
    }
  }
#endif
  return EXIT_SUCCESS;
}
