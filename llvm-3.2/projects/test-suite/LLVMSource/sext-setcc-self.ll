define <4 x i32> @test_ueq(<4 x float> %val) {
entry:
  %cmp = fcmp ueq <4 x float> %val, %val
  %mask = sext <4 x i1> %cmp to <4 x i32>
  ret <4 x i32> %mask
}

define <4 x i32> @test_uge(<4 x float> %val) {
entry:
  %cmp = fcmp uge <4 x float> %val, %val
  %mask = sext <4 x i1> %cmp to <4 x i32>
  ret <4 x i32> %mask
}

define <4 x i32> @test_ule(<4 x float> %val) {
entry:
  %cmp = fcmp ule <4 x float> %val, %val
  %mask = sext <4 x i1> %cmp to <4 x i32>
  ret <4 x i32> %mask
}

define <4 x i32> @test_one(<4 x float> %val) {
entry:
  %cmp = fcmp one <4 x float> %val, %val
  %mask = sext <4 x i1> %cmp to <4 x i32>
  ret <4 x i32> %mask
}

define <4 x i32> @test_ogt(<4 x float> %val) {
entry:
  %cmp = fcmp ogt <4 x float> %val, %val
  %mask = sext <4 x i1> %cmp to <4 x i32>
  ret <4 x i32> %mask
}

define <4 x i32> @test_olt(<4 x float> %val) {
entry:
  %cmp = fcmp olt <4 x float> %val, %val
  %mask = sext <4 x i1> %cmp to <4 x i32>
  ret <4 x i32> %mask
}

define i32 @main() nounwind {
  ; test unordered equality comparisons (all ones)
  %res_ueq = call <4 x i32> @test_ueq(<4 x float> undef)
  %res_uge = call <4 x i32> @test_uge(<4 x float> undef)
  %res_ule = call <4 x i32> @test_ule(<4 x float> undef)

  ; test ordered inequality comparisons (all zeros)
  %res_one = call <4 x i32> @test_one(<4 x float> undef)
  %res_ogt = call <4 x i32> @test_ogt(<4 x float> undef)
  %res_olt = call <4 x i32> @test_olt(<4 x float> undef)

  ; combine all results into a single mask (all ones)
  %tmp_uno = and <4 x i32> %res_uge, %res_ule
  %res_uno = and <4 x i32> %res_ueq, %tmp_uno
  %tmp_ord = or <4 x i32> %res_ogt, %res_olt
  %res_ord = or <4 x i32> %res_one, %tmp_ord
  %res_ord_inv = xor <4 x i32> %res_ord, <i32 -1, i32 -1, i32 -1, i32 -1>
  %res = and <4 x i32> %res_uno, %res_ord_inv

  ; compare result to -1,-1,-1,-1
  %res_scalar = bitcast <4 x i32> %res to i128
  %ret_cmp = icmp ne i128 %res_scalar, -1
  %ret_val = zext i1 %ret_cmp to i32
  ret i32 %ret_val
}
