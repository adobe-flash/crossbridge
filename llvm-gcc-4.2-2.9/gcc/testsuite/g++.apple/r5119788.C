typedef float vFloat __attribute__ ((__vector_size__ (16)));

typedef vFloat vfloat;

void foo() {
  vfloat mult_[4] = { { 1.0f, 1.0f, 1.0f, 1.0f },
		      { 2.0f, 2.0f, 2.0f, 2.0f },
		      { 3.0f, 3.0f, 3.0f, 3.0f },
		      { 4.0f, 4.0f, 4.0f, 4.0f } };
}
