void motion_test1(int *data)
{
  int i;
  if (data[1])
    i = data[0] + data[3];
  else {
    data[data[2]] = 2;
    i = 5;
  }
  data[3] = data[0] + data[3];
  data[4] = i;
}
void motion_result1(int *data)
{
  int i;
  int j;
  if (data[1]) {
    j = data[0] + data[3];
    i = j;
  }
  else {
    data[data[2]] = 2;
    i = 5;
    j = data[0] + data[3];
  }
  data[4] = j;
  data[5] = i;
}
void motion_test2(int *data)
{
  int j;
  int i = 1;
  if (data[1]) {
    data[data[2]] = 2;
    j = data[0] + data[3];
    i = i + j;
  }
  data[4] = data[0] + data[3];
  data[5] = i;
}
void motion_result2(int *data)
{
  int j;
  int i = 1;
  if (data[1]) {
    data[data[2]] = 2;
    j = data[0] + data[3];
    i = i + j;
  }
  else
    j = data[0] + data[3];
  data[4] = j;
  data[5] = i;
}
void motion_test3(int *data)
{
  int i = 0;
  int k = data[2];
  int j = data[0];
  do
    i = 21 * j + i + 1;
  while (i < k);
  data[4] = i;
}
void motion_result3(int *data)
{
  int i = 0;
  int k = data[2];
  int j = 21 * data[0];
  do
    i = j + i + 1;
  while (i < k);
  data[4] = i;
}
void motion_test4(int *data)
{
  int i = 0;
  int j = data[2];
  do {
    data[i] = 2 / j + i - 1;
    i++;
  } while (i < data[2]);
}
void motion_result4(int *data)
{
  int i = 0;
  int j = 2 / data[2];
  do {
    data[i] = j + i - 1;
    i++;
  } while (i < data[2]);
}
void motion_test5(int *data)
{
  int i = 0;
  int j = data[0];
  if (data[1])
    goto here;
  j = data[3] + j;
  do {
    i++;
here:
    data[i] = 21 * j + i;
  } while (i < data[2]);
}
void motion_result5(int *data)
{
  int i = 0;
  int j = data[0];
  if (data[1]) {
    j = j * 21;
    goto here;
  }
  j = data[3] + j;
  j = j * 21;
  do {
    i++;
here:
    data[i] = j + i;
  } while (i < data[2]);
}
void motion_test6(int *data)
{
  int j = data[1];
  int k = data[2];
  int i = data[0];
  int n = data[3];
  do
    i = j + i + k;
  while (i < n);
  data[4] = i;
}
void motion_result6(int *data)
{
  int j = data[1];
  int k = data[2];
  int i = data[0];
  int n = data[3];
  int m = j + k;
  do
    i = m + i;
  while (i < n);
  data[4] = i;
}
void motion_test7(int *data)
{
  int i = data[1];
  int j = data[0];
  do {
    if (i & 1) data[data[2]] = 21 * j + i;
    i++;
  } while (i < data[3]);
}
void motion_result7(int *data)
{
  int i = data[1];
  int j = data[0] * 21;
  do {
    if (i & 1) data[data[2]] = j + i;
    i++;
  } while (i < data[3]);
}
void motion_test8(int *data)
{
  int i = 0;
  int j = data[0];
  while (i < data[2]) {
    data[i] = 21 * j + i;
    i++;
  }
}
void motion_result8(int *data)
{
  int i = 0;
  int j = data[0];
  if (i < data[2]) {
    j = j * 21;
    do {
      data[i] = j + i;
      i++;
    } while (i < data[2]);
  }
}
void motion_test9(int *data)
{
  int i = 0;
  int j = data[0];
loop:
  if (i >= data[2]) return;
    data[i] = 21 * j + i;
    i++;
    goto loop;
}
void motion_result9(int *data)
{
  int i = 0;
  int j = data[0];
  if (i >= data[2]) return;
  j = j * 21;
loop:
    data[i] = j + i;
    i++;
  if (i < data[2]) goto loop;
}
void motion_test10(int *data)
{
  int j;
  int p = data[1];
  int i = data[0];
  do {
    if (p)
      j = 1;
    else
      j = 2;
    i = i + j;
    data[data[2]] = 2;
  } while (i < data[3]);
}
void motion_result10(int *data)
{
  int j;
  int p = data[1];
  int i = data[0];
  if (p)
    j = 1;
  else
    j = 2;
  do {
    i = i + j;
    data[data[2]] = 2;
  } while (i < data[3]);
}
void motion_test11(int *data)
{
  int p = data[1];
  int i = data[0];
  do {
    if (p)
      i = i + 1;
    else
      i = i + 2;
    data[data[2]] = 2;
  } while (i < data[3]);
}
void motion_result11(int *data)
{
  int p = data[1];
  int i = data[0];
  if (p)
    do {
      i = i + 1;
      data[data[2]] = 2;
    } while (i < data[3]);
  else
    do {
      i = i + 2;
      data[data[2]] = 2;
    } while (i < data[3]);
}
