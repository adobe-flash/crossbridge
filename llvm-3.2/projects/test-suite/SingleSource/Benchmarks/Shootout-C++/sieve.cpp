#include <iostream>
#include <vector>
#include <list>
#include <cstdlib>

using namespace std;

void sieve(list<int>& unknown, vector<int>& primes)
{
  while (!unknown.empty())
  {
    int p = unknown.front();
    unknown.pop_front();
    list<int>::iterator i = unknown.begin();
    while (i != unknown.end())
    {
      if (*i % p)
        ++i;
      else
        i = unknown.erase(i);
    }
    primes.push_back(p);
  }
}

int main(int argc, char *argv[]) 
{
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 50
#else
#define LENGTH 500
#endif
  size_t NUM = (argc == 2 ? (atoi(argv[1]) < 1 ? 1 : atoi(argv[1])): LENGTH);

  vector<int> primes;

  // run the sieve repeatedly
  while (NUM--) {
    list<int> integers;
    for (int i = 2; i < 8192; ++i)
      integers.push_back(i);
    primes.clear();
    sieve(integers, primes);
  }

  cout << "Count: " << primes.size() << endl;
  return 0;
}
