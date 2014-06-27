void dead_test1(int *data)
{
  int j = data[0] + data[1];
  j = data[2];
  data[j] = 2;
}
void dead_result1(int *data)
{
  int j;
  j = data[2];
  data[j] = 2;
}
void dead_test2(int *data)
{
  int j = data[0] + data[1];
  int k = j + data[2];
  int m = k + data[3];
  int n = m + data[4];
  j = data[2];
  data[j] = 2;
}
void dead_result2(int *data)
{
  int j;
  j = data[2];
  data[j] = 2;
}
void dead_test3(int *data)
{
  int k = 0;
  int j = data[1];
  if (j) {
    k++;
    j = k + data[0] * j;
  }
  else
    k--;
  j = data[2];
  data[j] = 2;
  data[3] = k;
}
void dead_result3(int *data)
{
  int k = 0;
  int j = data[1];
  if (j)
    k++;
  else
    k--;
  j = data[2];
  data[j] = 2;
  data[3] = k;
}
void dead_test4(int *data)
{
  int k = 0;
  int j = data[1];
  if (j) {
    k++;
    j = k + j * data[0];
  }
  else
    k--;
  if (data[4] & 1)
    k++;
  else {
    k--;
    j++;
  }
  j = data[2];
  data[j] = 2;
  data[3] = k;
}
void dead_result4(int *data)
{
  int k = 0;
  int j = data[1];
  if (j)
    k++;
  else
    k--;
  if (data[4] & 1)
    k++;
  else
    k--;
  j = data[2];
  data[j] = 2;
  data[3] = k;
}
void dead_test5(int *data)
{
  int i, j;
  int k = 0;
  int stop = data[0];
  for (i=0; i<stop; i++) {
    k = k * data[1];
    j = data[2];
    data[j] = 2;
  }
}
void dead_result5(int *data)
{
  int i, j;
  int stop = data[0];
  for (i=0; i<stop; i++) {
    j = data[2];
    data[j] = 2;
  }
}
void dead_test6(int *data)
{
  int i;
  int k = 0;
  int m = 0;
  int n = 0;
  int stop = data[0];
  for (i=0; i<stop; i++) {
    int j = data[1];
    if (j) {
      n = j * stop + m;
      k++;
    }
    else {
      m = n * stop + k;
      k--;
    }
    j = data[2];
    data[j] = 2;
  }
  data[3] = k;
}
void dead_result6(int *data)
{
  int i;
  int k = 0;
  int m = 0;
  int n = 0;
  int stop = data[0];
  for (i=0; i<stop; i++) {
    int j = data[1];
    if (j)
      k++;
    else
      k--;
    j = data[2];
    data[j] = 2;
  }
  data[3] = k;
}
void dead_test7(int *data)
{
  int k = 0;
  int j = data[2];
  data[j] = 2;
  if (j) k++;
  else k--;
}
void dead_result7(int *data)
{
  int j = data[2];
  data[j] = 2;
}
void dead_test8(int *data)
{
  int i;
  int k = 0;
  int stop = data[0];
  for (i=0; i<stop; i++) {
    int j = data[1] + k;
    if (j) k++;
    else k--;
    j = data[2];
    data[j] = 2;
  }
}
void dead_result8(int *data)
{
  int i;

  int stop = data[0];
  for (i=0; i<stop; i++) {
    int j;
    j = data[2];
    data[j] = 2;
  }
}
void dead_test9(int *data)
{
  int k = 0;
  int j;
  for (j=0; j<5; j++) k++;
  j = data[2];
  data[j] = 2;
}
void dead_result9(int *data)
{
  int j;
  j = data[2];
  data[j] = 2;
}
void dead_test10(int *data)
{
  int k = 0;
  int j;
  for (j=0; j<data[0]; j++) k++;
  j = data[2];
  data[j] = 2;
}
void dead_result10(int *data)
{
  int j;
  j = data[2];
  data[j] = 2;
}
void dead_test11(int *data)
{
  int i = data[0];
  int j = data[1];
  int k = i * j;
  if (i & j)
    data[0] = k;
  j = data[2];
  data[j] = 2;
}
void dead_result11(int *data)
{
  int i = data[0];
  int j = data[1];
  if (i & j)
    data[0] = i * j;
  j = data[2];
  data[j] = 2;
}
