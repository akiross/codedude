__kernel void vector_add(__global const int *A, __global const int *B, __global int *C, const int len) {
	int tid = get_global_id(0);
	if (tid < len)
		C[tid] = A[tid] + B[tid];
}

