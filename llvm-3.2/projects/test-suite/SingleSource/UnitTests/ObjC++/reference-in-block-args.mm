extern "C" void abort (void);

struct ST {
     int filler; 
     int referrer;
};

void OUTER_BLOCK(void (^fixer)(ST& ref)) {
    ST ref = {2, 100};
    fixer(ref);
}
  
void INNER_BLOCK(int (^largeDo) ()) {
        if (largeDo() != 102)
	  abort();
}

void scan() {
            OUTER_BLOCK(^(ST &ref) {
                INNER_BLOCK(^() { 
                  if (ref.referrer != 100)
                    abort();
                  return ref.referrer + ref.filler; 
                });
            });

}

int main(int argc, const char *argv[]) {
    scan();
    return 0;
}
