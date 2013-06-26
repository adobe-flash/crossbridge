#if defined(__BLOCKS__) && defined(__clang__)
extern "C" void abort(void);

class Test {
public:
    int i;
    Test() : i(0)
    {
    }
    
    ~Test()
    {
	i = -1;
    }
    
    Test(const Test&r)
    {
	i = r.i;
    }
    
    void speak() const
    {
	if (i != 0)
	  abort();
    }
};
#endif

int main (int , char * const []) 
{
#if defined(__BLOCKS__) && defined(__clang__)
    Test t;
    ^{  t.speak();  }();
#endif
    return 0;
}    
