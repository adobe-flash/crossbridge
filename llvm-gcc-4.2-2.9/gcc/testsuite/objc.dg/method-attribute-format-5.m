/* APPLE LOCAL file 6157135 */
/* Check for declaration and usage of 'format' __NSString sttribuet where number of
   first variarg is 0 */
/* { dg-options "-Wformat" } */
/* { dg-do compile } */
#include <Foundation/Foundation.h>

@interface MyClass
- (id)initWithFormat:(NSString *)format arguments:(va_list)argList __attribute__((format(__NSString__, 1, 0)));
@end

void FOO (MyClass* p)
{
	[p initWithFormat:(NSString *)nil arguments: 0];
}
