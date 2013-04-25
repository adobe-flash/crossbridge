/* APPLE LOCAL file 10.5 debug mode 6621704 */
/* { dg-do run { target *-*-darwin* } } */
/* { dg-options "-isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5" } */

#define _GLIBCXX_DEBUG 1

#include <string>
#include <map>
#include <iostream>

std::string a;
std::map<int, int> m;

using namespace std;

string a1;
map<int, int> m1;

main() {
  m[1]=2;
  a += "World\n";
  m1[1]=2;
  a1 += "hi";
  cout << "Hello " << a;
}
