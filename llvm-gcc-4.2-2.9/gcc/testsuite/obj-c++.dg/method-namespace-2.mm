/* APPLE LOCAL file radar 4516785 */
/* Test that qualified type is resolved to its proper type. */
/* { dg-do compile } */

@interface  Object 
{
	int I[100];
}
@end

namespace HC
{

struct Object
{
    void test();
}; 

} // namespace HC

int main()
{
    HC::Object* object;
    object->test();	// Must compile with no error
    return 0;
}
