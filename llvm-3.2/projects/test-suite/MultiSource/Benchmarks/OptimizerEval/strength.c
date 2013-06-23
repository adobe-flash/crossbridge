void strength_test1(int *data)
{
  int i = 0;
  do {
    data[data[2]] = 2;
    i = i + 1;
  } while (i * 21 < data[1]);
}
void strength_result1(int *data)
{
  int i = 0;
  do {
    data[data[2]] = 2;
    i = i + 21;
  } while (i < data[1]);
}
void strength_test2(int *data)
{
  int k = data[0];
  int i = 0;
  do {
    data[data[2]] = 2;
    i = i + 1;
  } while (i * k < data[1]);
}
void strength_result2(int *data)
{
  int k = data[0];
  int i = 0;
  do {
    data[data[2]] = 2;
    i = i + k;
  } while (i < data[1]);
}
void strength_test3(int *data)
{
  int i = data[0];
  int j = data[1];
  do {
    data[data[2]] = 2;
    i = i + 1;
    j = j + 1;
  } while (i * j < data[3]);
}
void strength_result3(int *data)
{
  int i = data[0];
  int j = data[1];
  int k = i * j;
  do {
    data[data[2]] = 2;
    i = i + 1;
    k = k + j;
    j = j + 1;
    k = k + i;
  } while (k < data[3]);
}
void strength_test4(int *data)
{
  int i;
  if (data[1]) {
    i = 2;
    goto here;
  }
  i = 0;
  do {
    i = i + 1;
here:
    data[data[2]] = 2;
  } while (i * 21 < data[3]);
}
void strength_result4(int *data)
{
  int i;
  if (data[1]) {
    i = 42;
    goto here;
  }
  i = 0;
  do {
    i = i + 21;
here:
    data[data[2]] = 2;
  } while (i < data[3]);
}
void strength_test5(int *data)
{
  int i = 0;
  while (1) {
    i = i + 1;
    if (data[1] && i * 21 > data[3])
      break;
    data[data[2]] = 2;
  }
}
void strength_result5(int *data)
{
  int i = 0;
  while (1) {
    i = i + 21;
    if (data[1] && i > data[3])
      break;
    data[data[2]] = 2;
  }
}
void strength_test6(int *data)
{
  int j = data[0];
  int i = data[1];
  do {
    data[data[2]] = 2;
    i = i + j;
  } while (i * 21 < data[3]);
}
void strength_result6(int *data)
{
  int j = data[0] * 21;
  int i = data[1] * 21;
  do {
    data[data[2]] = 2;
    i = i + j;
  } while (i < data[3]);
}
void strength_test7(int *data)
{
  int i = 0;
  do {
    if (data[1])
      i = i + 1;
    else
      i = i + 2;
    data[data[2]] = 2;
  } while (i * 21 < data[3]);
}
void strength_result7(int *data)
{
  int i = 0;
  do {
    if (data[1])
      i = i + 21;
    else
      i = i + 42;
    data[data[2]] = 2;
  } while (i < data[3]);
}
void strength_test8(int *data)
{
  int i = 0;
  do {
    int j = i + 1;
    data[data[2]] = j;
    i = j + 1;
  } while (i * 21 < data[3]);
}
void strength_result8(int *data)
{
  int i = 0;
  int n = 0;
  do {
    int j = i + 1;
    n = n + 21;
    data[data[2]] = j;
    i = j + 1;
    n = n + 21;
  } while (n < data[3]);
}
void strength_test9(int *data)
{
  int i = 0;
  do {
    data[data[2]] = i;
    i = i + 1;
  } while (i * 21 < data[3]);
}
void strength_result9(int *data)
{
  int i = 0;
  int j = 0;
  do {
    data[data[2]] = i;
    i = i + 1;
    j = j + 21;
  } while (j < data[3]);
}
void strength_test10(int *data)
{
  int stop = data[3];
  int i = 0;
  do {
    data[data[2]] = 21 * i;
    i = i + 1;
  } while (i < stop);
}
void strength_result10(int *data)
{
  int stop = data[3] * 21;
  int i = 0;
  do {
    data[data[2]] = i;
    i = i + 21;
  } while (i < stop);
}
