int func(int a, int b, int c);

__kernel void vec_fma(__global int *out,
                      __global const int *in1,
                      __global const int *in2,
                      __global const int *in3) {
  int i = get_global_id(0);
  out[i] = func(in1[i], in2[i], in3[i]);
}
