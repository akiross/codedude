__kernel void vector_add(__global const int *R, __global const int *A, __global int *B, const int size) {
	int tid = get_global_id(0);
	
	if (tid < len)
		C[tid] = A[tid] + B[tid];
}

