void vnum_test1(int *data)
{
  data[0] = data[1] * data[3] - data[1] * data[3];
}
void vnum_result1(int *data)
{
  data[0] = 0;
}
void vnum_test2(int *data)
{
  data[0] = data[1] * data[3] - data[3] * data[1];
}
void vnum_result2(int *data)
{
  data[0] = 0;
}
void vnum_test3(int *data)
{
  int n;
  int j = data[1] * data[3];
  int i = data[3];
  int m = data[1];
  int k = data[2];
  data[k] = 2;
  n = m * i;
  data[0] = n - j;
}
void vnum_result3(int *data)
{
  int k = data[2];
  data[k] = 2;
  data[0] = 0;
}
void vnum_test4(int *data)
{
  int n;
  int j = data[1] * data[3];
  int i = data[3];
  int m = data[1];
  int k = data[2];
  data[k] = 2;
  n = i * m;
  data[0] = n - j;
}
void vnum_result4(int *data)
{
  int k = data[2];
  data[k] = 2;
  data[0] = 0;
}
void vnum_test5(int *data)
{
  int j = data[1] * data[3];
  if (data[3] == 3) {
    int n;
    int i = data[3];
    int m = data[1];
    int k = data[2];
    data[k] = 2;
    n = m * i;
    data[0] = n - j;
  }
  else if (data[0] & 1) {
    j = 3 + data[2] - j;
    data[j] = 2;
  }
}
void vnum_result5(int *data)
{
  int j = data[1] * data[3];
  if (data[3] == 3) {
    int k = data[2];
    data[k] = 2;
    data[0] = 0;
  }
  else if (data[0] & 1) {
    j = 3 + data[2] - j;
    data[j] = 2;
  }
}
void vnum_test6(int *data)
{
  int n;
  int j = data[1] * data[3];
  int m = data[1];
  int k = j;
  if (data[0])
    j = j + 3;
  else
    j = j - 3;
  n = data[3];
  j = data[2] + j;
  data[j] = 2;
  data[4] = k - m * n;
}
void vnum_result6(int *data)
{
  int j = data[1] * data[3];
  if (data[0])
    j = j + 3;
  else
    j = j - 3;   
  j = data[2] + j;
  data[j] = 2;
  data[4] = 0;
}
void vnum_test7(int *data)
{
  int i, j, k;
  int m = data[1];
  int n = data[3];
  if (data[0]) {
    j = m * n;
    i = data[2];
    data[i] = 2;
    k = m * n;
  }
  else {
    j = 5;
    k = 5;
  }
  data[0] = k - j;
}
void vnum_result7(int *data)
{
  int i;
  if (data[0]) {
    i = data[2];
    data[i] = 2;
  }
  data[0] = 0;
}
void vnum_test8(int *data)
{
  int i;
  int stop = data[3];
  int m = data[4];
  int n = m;
  for (i=0; i<stop; i++) {
    int k = data[2];
    data[k] = 2;
    data[0] = m - n;
    k = data[1];
    m = m + k;
    n = n + k;
  }
}
void vnum_result8(int *data)
{
  int i;
  int stop = data[3];
  for (i=0; i<stop; i++) {
    int k = data[2];
    data[k] = 2;
    data[0] = 0;
  }
}
void vnum_test9(int *data)
{
  int i = data[0];
  int j = data[1];
  if (i == j)
    data[2] = (i - j) * 21;
  else
    data[2] = i + j;
}
void vnum_result9(int *data)
{
  int i = data[0];
  int j = data[1];
  if (i == j)
    data[2] = 0;
  else
    data[2] = i + j;
}
void vnum_test10(int *data)
{
  int i = data[0];
  int m = i + 1;
  int j = data[1];
  int n = j + 1;
  data[2] = m + n;
  if (i == j)
    data[3] = (m - n) * 21;
}
void vnum_result10(int *data)
{
  int i = data[0];
  int m = i + 1;
  int j = data[1];
  int n = j + 1;
  data[2] = m + n;
  if (i == j)
    data[3] = 0;
}
void vnum_test11(int *data)
{
  int n;
  int stop = data[3];
  int j = data[1];
  int k = j;
  int i = 1;
  for (n=0; n<stop; n++) {
    if (j != k) i = 2;
    if (i != 1) k = 2;
    data[data[2]] = 2;
  }
  data[1] = i;
}
void vnum_result11(int *data)
{
  int n;
  int stop = data[3];
  for (n=0; n<stop; n++)
    data[data[2]] = 2;
  data[1] = 1;
}
void vnum_test12(int *data)
{
  int n;
  int stop = data[3];
  int j = data[1];
  int k = j;
  int i = 1;
  for (n=0; n<stop; n++) {
    if (j != k) i = 2;
    i = 2 - i;
    if (i != 1) k = 2;
    data[data[2]] = 2;
  }
  data[1] = i;
}
void vnum_result12(int *data)
{
  int n;
  int stop = data[3];
  for (n=0; n<stop; n++)
    data[data[2]] = 2;
  data[1] = 1;
}
