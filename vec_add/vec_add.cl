__kernel void vec_add(__global int *out,
                      __global const int *in1,
                      __global const int *in2) {
  int i = get_global_id(0);
  out[i] = in1[i] + in2[i];
}
