#include <stdio.h>
#include <string.h>

void *malloc(size_t);

typedef unsigned short UChar;

static inline UChar toASCIILower(UChar c) { return c | ((c >= 'A' && c <= 'Z') << 5); }

static size_t lower_StringImp(const UChar* data, size_t m_length, UChar* output) __attribute__((__noinline__));
static size_t lower_StringImpl(const UChar* __restrict data, size_t length, UChar* __restrict output)
{
  // Do a faster loop for the case where all the characters are ASCII.
  UChar ored = 0;
  size_t i;
  for (i = 0; i < length; i++) {
    UChar c = data[i];
    ored |= c;
    output[i] = toASCIILower(c);
  }
  if (!(ored & ~0x7F))
    return 1;

  return 0;
}

static UChar staticData[]  = {'H', 'E', 'L', 'L', 'O', ' ', 'W', 'O', 'R', 'L', 'D', '!', 'H', 'E', 'L', 'L'};
static const size_t staticDataLength = sizeof(staticData) / sizeof(staticData[0]);

static void doTest(size_t numberOfIterations)
{
  const size_t numberOfCharacters = numberOfIterations;
  const size_t testDataLength = ((numberOfCharacters + staticDataLength - 1) / staticDataLength) * staticDataLength;
  UChar* testData = malloc(sizeof(UChar) * testDataLength);
  size_t i;
  for (i = 0; i < testDataLength; i += staticDataLength)
    memcpy(testData + i, staticData, staticDataLength * sizeof(staticData[0]));

  UChar* result = malloc(sizeof(UChar) * testDataLength);
  printf("iterations (%ld characters)\n", numberOfIterations, numberOfCharacters);

  memset(result, 0, sizeof(UChar) * testDataLength);
#ifdef SMALL_PROBLEM_SIZE
  for (i = 0; i < 100000; i++)
#else
  for (i = 0; i < 10000000; i++)
#endif
    lower_StringImpl(testData, numberOfCharacters, result);
}

int main(int argc, char **argv)
{
  size_t i;
  for (i = 0; i < 32; i++)
    doTest(i);

  return 0;
}
