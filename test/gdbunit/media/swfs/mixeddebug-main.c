
extern void func_with_debuginfo_A(int arg);
extern void func_with_debuginfo_B(int arg);
extern void func_with_debuginfo_C(int arg);
extern void func_with_no_debuginfo_A(int arg);
extern void func_with_no_debuginfo_B(int arg);
extern void func_with_no_debuginfo_C(int arg);

int main()
{
	func_with_no_debuginfo_A(42);
}