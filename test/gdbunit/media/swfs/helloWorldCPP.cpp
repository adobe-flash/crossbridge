// A simple C++ hello world application for simple debugging.

#include <iostream>
using namespace std;

int square(int a);

int a = 5;
int *aPointer = &a;
char letters[5] = {'A', 'B', 'C', 'D', 'E'};
int numbers[5] = {1, 2, 3, 4, 5};

int square(int a)
{
    return a * a;
}

int main ()
{
    int s = 2;
    int t = 1;
    
    int i = 0;
    for (i = 0; i < 8; i++){
        t *= 2;
    }

    cout << "Hello World C++" << endl;
    cout << s << "*" << s << "=" << square(s) << endl;

    // try an exception
    try {
        int et = 0; 
    	int es = 1; 
        
    	for (et = 0; et < 10; et++){ 
            cout << et << endl; 
    	} 
        
        int squared = square(et);
    	cout << es << "*" << es << "=" << squared << endl; 
        
    	// go into the catch 
        throw 20;
    } catch (int e) {
        cout << "An exception occurred. Exception Number: " << e << endl;
    }

    return 0;
}
