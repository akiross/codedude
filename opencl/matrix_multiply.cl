__kernel void square_matrix_multiply(__global int *C, __global const int *A, __global const int *B, const int side) {
	int row = get_global_id(0);
	int col = get_global_id(1);
	
	C[row * side + col] = 0;
	for (int i = 0; i < side; ++i)
		C[row * side + col] += A[row * side + i] * B[i * side + col];
}

