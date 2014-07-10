extern void func_with_debuginfo_A(int arg);
extern void func_with_debuginfo_B(int arg);
extern void func_with_debuginfo_C(int arg);

void func_with_no_debuginfo_A(int arg)
{
	printf("func_with_no_debuginfo_A %d\n", arg);
	func_with_debuginfo_A(arg);
}

void func_with_no_debuginfo_B(int arg)
{
	printf("func_with_no_debuginfo_B %d\n", arg);
	func_with_debuginfo_B(arg);
}

void func_with_no_debuginfo_C(int arg)
{
	printf("func_with_no_debuginfo_C %d\n", arg);
	func_with_debuginfo_C(arg);
}
