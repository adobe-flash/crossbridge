void cprop_test1(int *data)
{
  int j = 1 + 2 * 4;
  data[0] = j;
}
void cprop_result1(int *data)
{
  int j = 9;
  data[0] = j;
}
void cprop_test2(int *data)
{
  int j = 1;
  int k = 2;
  int m = 4;
  int n = k * m;
  j = n + j;
  data[0] = j;
}
void cprop_result2(int *data)
{
  int j;
  j = 9;
  data[0] = j;
}
void cprop_test3(int *data)
{
  int j = 12345;
  if (data[0])
    data[1] = 1 + j - 1234;
  else
    data[2] = 123 + j + 10;
}
void cprop_result3(int *data)
{
  if (data[0])
    data[1] = 11112;
  else
    data[2] = 12478;
}
void cprop_test4(int *data)
{
  int j = 12345;
  if (data[0])
    data[1] = 1 + j - 1234;
  else
    data[2] = 123 - j + 10;
}
void cprop_result4(int *data)
{

  if (data[0])
    data[1] = 11112;
  else
    data[2] = -12212;
}
void cprop_test5(int *data)
{
  int j = 5;
  if (data[0])
    data[1] = 10;
  else
    data[2] = 15;
  data[3] = j + 21;
}
void cprop_result5(int *data)
{
  if (data[0])
    data[1] = 10;
  else
    data[2] = 15;
  data[3] = 26;
}
void cprop_test6(int *data)
{
  int j = 5;
  if (data[0])
    data[1] = j * 10;
  else
    data[2] = j * 15;
  data[3] = j * 21;
}
void cprop_result6(int *data)
{
  if (data[0])
    data[1] = 50;
  else
    data[2] = 75;
  data[3] = 105;
}
void cprop_test7(int *data)
{
  int j;
  if (data[0]) {
    j = 5;
    data[1] = 10;
  }
  else {
    data[2] = 15;
    j = 5;
  }
  data[3] = j + 21;
}
void cprop_result7(int *data)
{
  if (data[0])
    data[1] = 10;
  else
    data[2] = 15;
  data[3] = 26;
}
void cprop_test8(int *data)
{
  int j, k;
  if (data[0]) {
    j = 4;
    k = 6;
    data[1] = j;
  }
  else {
    j = 7;
    k = 3;
    data[2] = k;
  }
  data[3] = (j + k) * 21;
}
void cprop_result8(int *data)
{
  if (data[0])
    data[1] = 4;
  else
    data[2] = 3;
  data[3] = 210;
}
void cprop_test9(int *data)
{
  int i;
  int stop = data[0];
  int j = 21;
  for (i=1; i<stop; i++)
    j = (j - 20) * 21;
  data[1] = j;
  data[2] = i;
}
void cprop_result9(int *data)
{
  int i;
  int stop = data[0];
  for (i=1; i<stop; i++)
    ;
  data[1] = 21;
  data[2] = i;
}
void cprop_test10(int *data)
{
  int j = 1;
  if (j) j = 10;
  else   j = data[0];
  data[0] = j * 21 + data[1];
}
void cprop_result10(int *data)
{
  data[0] = 210 + data[1];
}
void cprop_test11(int *data)
{
  int i;
  int stop = data[0];
  int j = 1;
  for (i=1; i<stop; i++)
    if (!j) j = i;
  data[1] = j;
  data[2] = i;
}
void cprop_result11(int *data)
{
  int i;
  int stop = data[0];
  for (i=1; i<stop; i++)
    ;
  data[1] = 1;
  data[2] = i;
}
void cprop_test12(int *data)
{
  int j = data[0];
  if (j == 5)
    j = j * 21 + 25 / j;
  data[1] = j;
}
void cprop_result12(int *data)
{
  int j = data[0];
  if (j == 5)
    j = 110;
  data[1] = j;
}
void cprop_test13(int *data)
{
  int j = data[1];
  int k = data[0];
  if (j == 5 && k == 10)
    j = j * 21 + 100 / k;
  data[2] = j;
}
void cprop_result13(int *data)
{
  int j = data[1];
  int k = data[0];
  if (j == 5 && k == 10)
    j = 115;
  data[2] = j;
}
void cprop_test14(int *data)
{
  int i = 10;
  int j = data[0];
  int k = 20;
  int m = data[1];
  int n = 30;
  if (data[2])
    data[3] = i + j + k + m + n;
  else
    data[0] = i * (j - k * (m - n));
}
void cprop_result14(int *data)
{
  int j = data[0];
  int m = data[1];
  if (data[2])
    data[3] = 60 + j + m;
  else
    data[0] = 10 * (j - 20 * (m - 30));
}
