/* APPLE LOCAL file Radar 4539933 */

extern int result;

class A
{
public:

    A() {++result;}

    void f()
    {
    }

private:

    char* mString;
};


template<typename T>
class Test
{
public:

    Test()
    {
        sA.f();
    }

private:
    static A sA;
};

template<typename T>
A Test<T>::sA;
