/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html )
    
    
    Source file for tests shared among several benchmarks
*/

/******************************************************************************/

template<typename T>
inline bool tolerance_equal(T &a, T &b) {
	T diff = a - b;
	return (abs(diff) < 1.0e-6);
}


template<>
inline bool tolerance_equal(int32_t &a, int32_t &b) {
	return (a == b);
}
template<>
inline bool tolerance_equal(uint32_t &a, uint32_t &b) {
	return (a == b);
}
template<>
inline bool tolerance_equal(uint64_t &a, uint64_t &b) {
	return (a == b);
}
template<>
inline bool tolerance_equal(int64_t &a, int64_t &b) {
	return (a == b);
}

template<>
inline bool tolerance_equal(double &a, double &b) {
	double diff = a - b;
	double reldiff = diff;
	if (fabs(a) > 1.0e-8)
		reldiff = diff / a;
	return (fabs(reldiff) < 1.0e-6);
}

template<>
inline bool tolerance_equal(float &a, float &b) {
	float diff = a - b;
	double reldiff = diff;
	if (fabs(a) > 1.0e-4)
		reldiff = diff / a;
	return (fabs(reldiff) < 1.0e-3);		// single precision divide test is really imprecise
}

/******************************************************************************/

template <typename T, typename Shifter>
inline void check_shifted_sum(T result) {
	T temp = (T)SIZE * Shifter::do_shift((T)init_value);
	if (!tolerance_equal<T>(result,temp))
		printf("test %i failed\n", current_test);
}

template <typename T, typename Shifter>
inline void check_shifted_sum_CSE(T result) {
	T temp = (T)0.0;
	if (!tolerance_equal<T>(result,temp))
		printf("test %i failed\n", current_test);
}

template <typename T, typename Shifter>
inline void check_shifted_variable_sum(T result, T var) {
	T temp = (T)SIZE * Shifter::do_shift((T)init_value, var);
	if (!tolerance_equal<T>(result,temp))
		printf("test %i failed\n", current_test);
}

template <typename T, typename Shifter>
inline void check_shifted_variable_sum(T result, T var1, T var2, T var3, T var4) {
	T temp = (T)SIZE * Shifter::do_shift((T)init_value, var1, var2, var3, var4);
	if (!tolerance_equal<T>(result,temp))
		printf("test %i failed\n", current_test);
}

template <typename T, typename Shifter>
inline void check_shifted_variable_sum_CSE(T result, T var) {
	T temp = (T)0.0;
	if (!tolerance_equal<T>(result,temp))
		printf("test %i failed\n", current_test);
}

template <typename T, typename Shifter>
inline void check_shifted_variable_sum_CSE(T result, T var1, T var2, T var3, T var4) {
	T temp = (T)0.0;
	if (!tolerance_equal<T>(result,temp))
		printf("test %i failed\n", current_test);
}


/******************************************************************************/

template <typename Iterator, typename T>
void fill(Iterator first, Iterator last, T value) {
	while (first != last) *first++ = value;
}

/******************************************************************************/

template <typename T>
	struct custom_constant_add {
	  static T do_shift(T input) { return (input + T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiple_constant_add {
	  static T do_shift(T input) { return (input + T(1) + T(2) + T(3) + T(4)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_constant_sub {
	  static T do_shift(T input) { return (input - T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiple_constant_sub {
	  static T do_shift(T input) { return (input - T(1) - T(2) - T(3) - T(4)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_constant_multiply {
	  static T do_shift(T input) { return (input * T(120)); }
	};

/******************************************************************************/

// this should result in a single multiply
template <typename T>
	struct custom_multiple_constant_multiply {
	  static T do_shift(T input) { return (input * T(2) * T(3) * T(4) * T(5)); }
	};

/******************************************************************************/

// this should result in a single add
template <typename T>
	struct custom_multiple_constant_multiply2 {
	  static T do_shift(T input) { return (input + T(2) * T(3) * T(4) * T(5)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_constant_divide {
	  static T do_shift(T input) { return (input / T(5)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiple_constant_divide {
	  static T do_shift(T input) { return ((((input / T(2) ) / T(3) ) / T(4)) / T(5)); }
	};

/******************************************************************************/

// this more likely to have constants fused than the version above
template <typename T>
	struct custom_multiple_constant_divide2 {
	  static T do_shift(T input) { return (input + (((T(120) / T(3) ) / T(4)) / T(5))); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiple_constant_mixed {
	  static T do_shift(T input) { return (input + T(2) - T(3) * T(4) / T(5)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_constant_and {
	  static T do_shift(T input) { return (input & T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiple_constant_and {
	  static T do_shift(T input) { return (input & T(15) & T(30) & T(31) & T(63)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_constant_or {
	  static T do_shift(T input) { return (input | T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiple_constant_or {
	  static T do_shift(T input) { return (input | T(15) | T(30) | T(31) | T(63)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_constant_xor {
	  static T do_shift(T input) { return (input ^ T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiple_constant_xor {
	  static T do_shift(T input) { return (input ^ T(15) ^ T(30) ^ T(31) ^ T(63)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_two {
	  static T do_shift(T input) { return (T(2)); }
	};

/******************************************************************************/
	
template <typename T>
	struct custom_add_constants {
	  static T do_shift(T input) { return (T(1) + T(2)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_sub_constants {
	  static T do_shift(T input) { return (T(2) - T(1)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiply_constants {
	  static T do_shift(T input) { return (T(2) * T(3)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_divide_constants {
	  static T do_shift(T input) { return (T(20) / T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_mod_constants {
	  static T do_shift(T input) { return (T(23) % T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_and_constants {
	  static T do_shift(T input) { return (T(23) & T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_or_constants {
	  static T do_shift(T input) { return (T(23) | T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_xor_constants {
	  static T do_shift(T input) { return (T(23) ^ T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_equal_constants {
	  static T do_shift(T input) { return (T(23) == T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_notequal_constants {
	  static T do_shift(T input) { return (T(23) != T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_greaterthan_constants {
	  static T do_shift(T input) { return (T(23) > T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_lessthan_constants {
	  static T do_shift(T input) { return (T(23) < T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_greaterthanequal_constants {
	  static T do_shift(T input) { return (T(23) >= T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_lessthanequal_constants {
	  static T do_shift(T input) { return (T(23) <= T(10)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_add_variable {
	  static T do_shift(T input, T v1) { return (input + v1); }
	};

/******************************************************************************/

template <typename T>
	struct custom_sub_variable {
	  static T do_shift(T input, T v1) { return (input - v1); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiply_variable {
	  static T do_shift(T input, T v1) { return (input * v1); }
	};

/******************************************************************************/

template <typename T>
	struct custom_divide_variable {
	  static T do_shift(T input, T v1) { return (input / v1); }
	};

/******************************************************************************/

template <typename T>
	struct custom_add_multiple_variable {
	  static T do_shift(T input, T v1, T v2, T v3, T v4) { return (input + v1 + v2 + v3 + v4); }
	};

/******************************************************************************/

template <typename T>
	struct custom_sub_multiple_variable {
	  static T do_shift(T input, T v1, T v2, T v3, T v4) { return (input - v1 - v2 - v3 - v4); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiply_multiple_variable {
	  static T do_shift(T input, T v1, T v2, T v3, T v4) { return (input * v1 * v2 * v3 * v4); }
	};

/******************************************************************************/

// something more likely to be moved out of loops, and a sanity check
template <typename T>
	struct custom_multiply_multiple_variable2 {
	  static T do_shift(T input, T v1, T v2, T v3, T v4) { return (input + v1 * v2 * v3 * v4); }
	};

/******************************************************************************/

// this can NOT have CSE and loop invariant motion applied in integer math
// and can only be optimized in float if inexact math is allowed
template <typename T>
	struct custom_divide_multiple_variable {
	  static T do_shift(T input, T v1, T v2, T v3, T v4) { return ((((input / v1 ) / v2 ) / v3) / v4); }
	};

/******************************************************************************/

// this can have CSE and loop invariant motion applied in integer math
// this should be optimizeable without inexact math
template <typename T>
	struct custom_divide_multiple_variable2 {
	  static T do_shift(T input, T v1, T v2, T v3, T v4) { return (input + (((v1 / v2 ) / v3) / v4)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_mixed_multiple_variable {
	  static T do_shift(T input, T v1, T v2, T v3, T v4) { return (input + v1 - v2 * v3 / v4); }
	};

/******************************************************************************/

template <typename T>
	struct custom_variable_and {
	  static T do_shift(T input, T v1) { return (input & v1); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiple_variable_and {
	  static T do_shift(T input, T v1, T v2, T v3, T v4) { return (input & v1 & v2 & v3 & v4); }
	};

/******************************************************************************/

template <typename T>
	struct custom_variable_or {
	  static T do_shift(T input, T v1) { return (input | v1); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiple_variable_or {
	  static T do_shift(T input, T v1, T v2, T v3, T v4) { return (input | v1 | v2 | v3 | v4); }
	};

/******************************************************************************/

template <typename T>
	struct custom_variable_xor {
	  static T do_shift(T input, T v1) { return (input ^ v1); }
	};

/******************************************************************************/

template <typename T>
	struct custom_multiple_variable_xor {
	  static T do_shift(T input, T v1, T v2, T v3, T v4) { return (input ^ v1 ^ v2 ^ v3 ^ v4); }
	};


/******************************************************************************/

template <typename T>
	struct custom_identity {
	  static T do_shift(T input) { return (input); }
	};

/******************************************************************************/

template <typename T>
	struct custom_add_zero {
	  static T do_shift(T input) { return (input + T(0)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_sub_zero {
	  static T do_shift(T input) { return (input - T(0)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_negate {
	  static T do_shift(T input) { return (-input); }
	};

/******************************************************************************/

template <typename T>
	struct custom_negate_twice {
	  static T do_shift(T input) { return (-(-input)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_zero_minus {
	  static T do_shift(T input) { return (T(0) - input); }
	};

/******************************************************************************/

template <typename T>
	struct custom_times_one {
	  static T do_shift(T input) { return (input * T(1)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_divideby_one {
	  static T do_shift(T input) { return (input / T(1)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_algebra_mixed {
	  static T do_shift(T input) { return (-(T(0) - (((input + T(0)) - T(0)) / T(1)))) * T(1); }
	};

/******************************************************************************/

template <typename T>
	struct custom_zero {
	  static T do_shift(T input) { return T(0); }
	};

/******************************************************************************/

template <typename T>
	struct custom_times_zero {
	  static T do_shift(T input) { return (input * T(0)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_subtract_self {
	  static T do_shift(T input) { return (input - input); }
	};
/******************************************************************************/

template <typename T>
	struct custom_algebra_mixed_constant {
	  static T do_shift(T input) { return (input - (-(T(0) - (((input + T(0)) / T(1)) - T(0)))) * T(1)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_cse1 {
	  static T do_shift(T v1, T v2, T v3) { return (v1 * (v2 - v3) ); }
	};

/******************************************************************************/

template <typename T>
	struct custom_and_self {
	  static T do_shift(T input) { return (input & input); }
	};

/******************************************************************************/

template <typename T>
	struct custom_or_self {
	  static T do_shift(T input) { return (input | input); }
	};

/******************************************************************************/

template <typename T>
	struct custom_xor_self {
	  static T do_shift(T input) { return (input ^ input); }
	};

/******************************************************************************/

template <typename T>
	struct custom_or_zero {
	  static T do_shift(T input) { return (input | T(0)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_xor_zero {
	  static T do_shift(T input) { return (input ^ T(0)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_andnot_zero {
	  static T do_shift(T input) { return (input & ~ T(0)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_and_zero {
	  static T do_shift(T input) { return (input & T(0)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_mod_one {
	  static T do_shift(T input) { return (input % T(1)); }
	};

/******************************************************************************/

template <typename T>
	struct custom_equal_self {
	  static T do_shift(T input) { return (input == input); }
	};

/******************************************************************************/

template <typename T>
	struct custom_notequal_self {
	  static T do_shift(T input) { return (input != input); }
	};

/******************************************************************************/

template <typename T>
	struct custom_greaterthan_self {
	  static T do_shift(T input) { return (input > input); }
	};

/******************************************************************************/

template <typename T>
	struct custom_lessthan_self {
	  static T do_shift(T input) { return (input < input); }
	};

/******************************************************************************/

template <typename T>
	struct custom_greaterthanequal_self {
	  static T do_shift(T input) { return (input >= input); }
	};

/******************************************************************************/

template <typename T>
	struct custom_lessthanequal_self {
	  static T do_shift(T input) { return (input <= input); }
	};

/******************************************************************************/

template <typename T, typename Shifter>
void test_constant(T* first, int count, const char *label) {
  int i;
  
  start_timer();
  
  for(i = 0; i < iterations; ++i) {
    T result = 0;
    for (int n = 0; n < count; ++n) {
		result += Shifter::do_shift( first[n] );
	}
    check_shifted_sum<T, Shifter>(result);
  }
  
  record_result( timer(), label );
}

/******************************************************************************/

template <typename T, typename Shifter>
void test_variable1(T* first, int count, T v1, const char *label) {
  int i;
  
  start_timer();
  
  for(i = 0; i < iterations; ++i) {
    T result = 0;
    for (int n = 0; n < count; ++n) {
		result += Shifter::do_shift( first[n], v1 );
	}
    check_shifted_variable_sum<T, Shifter>(result, v1);
  }
  
  record_result( timer(), label );
}

/******************************************************************************/

template <typename T, typename Shifter>
void test_variable4(T* first, int count, T v1, T v2, T v3, T v4, const char *label) {
  int i;
  
  start_timer();
  
  for(i = 0; i < iterations; ++i) {
    T result = 0;
    for (int n = 0; n < count; ++n) {
		result += Shifter::do_shift( first[n], v1, v2, v3, v4 );
	}
    check_shifted_variable_sum<T, Shifter>(result, v1, v2, v3, v4);
  }
  
  record_result( timer(), label );
}

/******************************************************************************/

template <typename T, typename Shifter>
void test_CSE_opt(T* first, int count, T v1, const char *label) {
  int i;
  
  start_timer();
  
  for(i = 0; i < iterations; ++i) {
    T result = 0;
	T temp = Shifter::do_shift( v1, first[0], first[1] );
	temp += temp;
	result += first[0] + temp;
	result -= first[1] + temp;
    for (int n = 1; n < count; ++n) {
		temp = Shifter::do_shift( v1, first[n-1], first[n] );
		temp += temp;
		result += first[n-1] + temp;
		result -= first[n] + temp;
	}
    check_shifted_variable_sum_CSE<T, Shifter>(result, v1);
  }
  
  record_result( timer(), label );
}

/******************************************************************************/

template <typename T, typename Shifter>
void test_CSE(T* first, int count, T v1, const char *label) {
  int i;
  
  start_timer();
  
  for(i = 0; i < iterations; ++i) {
    T result = 0;
	result += first[0] + Shifter::do_shift( v1, first[0], first[1] ) + Shifter::do_shift( v1, first[0], first[1] );
	result -= first[1] + Shifter::do_shift( v1, first[0], first[1] ) + Shifter::do_shift( v1, first[0], first[1] );
    for (int n = 1; n < count; ++n) {
		result += first[n-1] + Shifter::do_shift( v1, first[n-1], first[n] ) + Shifter::do_shift( v1, first[n-1], first[n] );
		result -= first[n] + Shifter::do_shift( v1, first[n-1], first[n] ) + Shifter::do_shift( v1, first[n-1], first[n] );
	}
    check_shifted_variable_sum_CSE<T, Shifter>(result, v1);
  }
  
  record_result( timer(), label );
}

/******************************************************************************/
