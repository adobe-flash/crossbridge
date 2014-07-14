class MamaDebugTest {
private:
  int N;
  
protected:
  MamaDebugTest(int n) : N(n) {}
  
  int getN() const { return N; }

};

class BabyDebugTest : public MamaDebugTest {
private:

public:
  BabyDebugTest(int n) : MamaDebugTest(n) {}
  
  static int doh;
  
  int  doit() {
    int N = getN();
    int Table[N];
    
    int sum = 0;
    
    for (int i = 0; i < N; ++i) {
      int j = i;
      Table[i] = j;
    }
    for (int i = 0; i < N; ++i) {
      int j = Table[i];
      sum += j;
    }
    
    return sum;
  }

};

int BabyDebugTest::doh;


int main(int argc, const char *argv[]) {
  BabyDebugTest BDT(20);
  BDT.doit();
  return 0;
}
