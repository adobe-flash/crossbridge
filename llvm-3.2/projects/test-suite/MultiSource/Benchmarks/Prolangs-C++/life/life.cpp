// Section 9.6, "Object-Oriented Programming using C++"
// by Ira Pohl.  Benjamin/Cummings Publishing Company, 1993

const int N=40, STATES=4;			// size of square board
const int DRAB=3;
const int DFOX=8;
#ifdef SMALL_PROBLEM_SIZE
const int CYCLES=1000;
#else
const int CYCLES=10000;
#endif
enum state { EMPTY, GRASS, RABBIT, FOX };

class living;					// forward decl

typedef living *world[N][N];			// world is simulation

class living {					// what lives in world
protected:
  int row, column;                              // location
  void sums(world w, int sm[]);           // sm[#states] used by next()
public:
  living (int r, int c) : row(r), column(c) {}
  virtual state who() = 0;                      // state identification
  virtual living *next(world w) = 0;            // compute next
};

void living::sums(world w, int sm[])
{
  int i,j;
  sm[EMPTY] = sm[GRASS] = sm[RABBIT] = sm[FOX] = 0;

  for (i = -1; i <= 1; ++i)
    for (j = -1; j <= 1; ++j)
      sm[w[row+i][column+j]->who()]++;
}

class fox : public living {
protected:
  int age;
public:
  fox(int r, int c, int a=0) : living(r,c), age(a) {}
  state who() {return (FOX);}
  living *next(world w);
};

class rabbit : public living {
protected:
  int age;
public:
  rabbit(int r, int c, int a=0) : living(r,c), age(a) {}
  state who() {return (RABBIT);}
  living *next(world w);
};

class grass : public living {
public:
  grass(int r, int c) : living(r,c) {}
  state who() {return (GRASS);}
  living *next(world w);
};

class empty : public living {
public:
  empty(int r, int c) : living(r,c) {}
  state who() {return (EMPTY);}
  living *next(world w);
};


living *grass::next(world w)
{
  int sum[STATES];
  sums(w, sum);

  if (sum[GRASS] > sum[RABBIT]) // eat grass
    return (new grass(row, column));
  else
    return (new empty(row, column));
}

living *rabbit::next(world w)
{
  int sum[STATES];
  sums(w, sum);

  if (sum[FOX] >= sum[RABBIT]) // eat rabbits
    return (new empty(row, column));
  else if (age > DRAB)         // rabbit too old
    return (new empty(row, column));
  else
    return (new rabbit(row, column, age+1));
}

living *fox::next(world w)
{
  int sum[STATES];
  sums(w, sum);

  if (sum[FOX] > 5)            // too many foxes
    return (new empty(row, column));
  else if (age > DFOX)         // fox too old
    return (new empty(row, column));
  else
    return (new fox(row, column, age+1));
}

living *empty::next(world w)
{
  int sum[STATES];
  sums(w, sum);

  if (sum[FOX] > 1)
    return (new fox(row, column));
  else if (sum[RABBIT] > 1)         // fox too old
    return (new rabbit(row, column));
  else if (sum[GRASS])
    return (new grass(row, column));
  else
    return (new empty(row, column));
}

// world is empty
void init(world w)
{
  int i,j;

  for (i = 0; i < N; ++i)
    for (j = 0; j < N; ++j)
      w[i][j] = new empty(i,j);
}

// new world from old world
void update(world w_new, world w_old)
{
  int i,j;

  for (i = 1; i < N-1; ++i)
    for (j = 1; j < N-1; ++j)
      w_new[i][j] = w_old[i][j]->next(w_old);
}

// clean world up
void dele(world w)
{
  int i,j;

  for (i = 1; i < N-1; ++i)
    for (j = 1; j < N-1; ++j)
      delete (w[i][j]);
}

void eden(world w)
{
  int i,j;

  for (i = 0; i < N; ++i)
    for (j = 0; j < N; ++j) {
      if (i == j) w[i][j] = new fox(i,j);
      else if (i < j) w[i][j] = new rabbit(i,j);
      else w[i][j] = new grass(i,j);
    }
}

// LLVM: add main return type.
int main ()
{
  world odd, even;
  int i;

  init(odd);

  eden(even);

  for (i = 0; i < CYCLES; ++i) { // simulation
    if (i % 2) {
      update(even,odd);
      dele(odd);
    }
    else {
      update(odd,even);
      dele(even);
    }
  }
}
