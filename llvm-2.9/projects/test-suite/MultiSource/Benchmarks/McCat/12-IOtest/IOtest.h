

struct global_result
{
  char min;
  char max;
  char add;
  char mult;
};

void initarray(void);

char array(unsigned long i);

unsigned long getac(void);

void setac(unsigned long i);

char min(char a, char b);

char max(char a, char b);

char add(char a, char b);

char mult(char a, char b);

void loop(void (*init)(void *) , void (*step)(void *) ,void *result);

 
