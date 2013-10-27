// Compiled with
// g++ -std=c++11 add_test_ochell.cpp -o add_test_ochell -l pocl && ./add_test_ochell
// or
// g++ -std=c++11 add_test_ochell.cpp -o add_test_ochell -l OpenCL && ./add_test_ochell

#include <iostream>
#include <cstdlib>

#include "ochell.hh"

int main(int argc, char **argv) {
	int len = 100;
	int *A = new int[len];
	int *B = new int[len];
	int *C = new int[len];
	
	for (int i = 0; i < len; ++i) {
		A[i] = 10 + i;
		B[i] = 100 + i;
	}

	// Create a context using the first device available
	cl::Context context = create_context(CL_DEVICE_TYPE_CPU);

	// Alocate buffers for I/O
	cl::Buffer inA = create_buffer(context, "rh", len, A);
	cl::Buffer inB = create_buffer(context, "rh", len, B);
	cl::Buffer outC = create_buffer(context, "wh", len, C);

	// Get a device handler
	std::vector<cl::Device> devices = get_devices(context);
	std::cout << "INFO: " << devices.size() << " devices available\n";
	
	// Create the program
	cl::Program program = load_and_build_program(
		context, devices, "vector_add_kernel.cl"
	);

	// Create a kernel object for accessing kernel entry point
	cl::Kernel ker_vec_add = load_kernel(program, "vector_add");

	// Set the arguments for this kernel (variadic template)
	set_kernel_args(ker_vec_add, inA, inB, outC, len);

	// Create a Command Queue for the device (1-to-1)
	cl::CommandQueue queue = create_command_queue(context, devices[0]);

	// Enqueue the kernel and get an event to check results
	cl::Event event = enqueue_nd_range_kernel(
		queue, ker_vec_add, cl::NullRange, cl::NDRange(len), cl::NDRange(1, 1)
	);
	
	// Wait for the conclusion
	event.wait();
	blocking_read_buffer(queue, outC, 0, len, C);
	
	for (int i = 0; i < len; ++i)
		std::cout << C[i] << " ";
	std::cout << std::endl;

	exit(EXIT_SUCCESS);
}

