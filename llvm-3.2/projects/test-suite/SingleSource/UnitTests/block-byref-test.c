#if defined(__BLOCKS__)
extern void abort();

int main() {
    __block unsigned char b1bool = 0;
    unsigned char *b1boolref = &b1bool;
    void(^b1)(void) = ^ {
        if (b1bool != 1)
          abort();
    };
    *b1boolref = 1;
    b1();
    
    __block unsigned char b2bool = 0;
    void(^b2)(void) = ^ {
        if (b2bool != 1)
          abort();
    };
    unsigned char *b2boolref = &b2bool;
    *b2boolref = 1;
    b2();
    return 0;
}
#else
int main() {
  return 0;
}
#endif
