// Compiled with
// g++ -std=c++11 matrix_mult_ochell.cpp -o matrix_mult_ochell -l pocl && ./matrix_mult_ochell
// or
// g++ -std=c++11 matrix_mult_ochell.cpp -o matrix_mult_ochell -l OpenCL && ./matrix_mult_ochell

#include <iostream>
#include <cstdlib>

#include "ochell.hh"

int main(int argc, char **argv) {
	int side = 5;
	int length = side * side;
	int *A = new int[length];
	int *B = new int[length];
	int *C = new int[length];
	size_t bsize = length * sizeof(int);

	for (int r = 0, i = 0; r < side; ++r)
		for (int c = 0; c < side; ++c, ++i) {
			A[i] = (r + 1) * 100 + c;
			B[i] = r == c ? 2 : 0;
		}

	// Context and program
	cl::Context ctx = create_context(CL_DEVICE_TYPE_ALL);
	std::vector<cl::Device> devs = get_devices(ctx);
	cl::Program prog = load_and_build_program(ctx, devs, "matrix_multiply.cl");
	cl::Kernel kern = load_kernel(prog, "square_matrix_multiply");

	// Buffer objects
	cl::Buffer inA = create_buffer(ctx, "rh", bsize, A);
	cl::Buffer inB = create_buffer(ctx, "rh", bsize, B);
	cl::Buffer outC = create_buffer(ctx, "wh", bsize, C);

	// Launch kernel
	set_kernel_args(kern, outC, inA, inB, side);
	cl::CommandQueue queue = create_command_queue(ctx, devs[0]);
	cl::Event event = enqueue_nd_range_kernel(
		queue, kern, cl::NullRange, cl::NDRange(side, side), cl::NDRange(1, 1)
	);
	event.wait();
	blocking_read_buffer(queue, outC, 0, bsize, C);
	
	// Print result matrix
	for (int r = 0, i = 0; r < side; ++r) {
		for (int c = 0; c < side; ++c, ++i)
			std::cout << C[i] << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;

	exit(EXIT_SUCCESS);
}

