// Compiled with
// g++ -std=c++11 add_test_ochell.cpp -o add_test_ochell -l pocl && ./add_test_ochell
// or
// g++ -std=c++11 add_test_ochell.cpp -o add_test_ochell -l OpenCL && ./add_test_ochell

#include <CL/cl.hpp> // opencl.h on mac
#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <cerrno>

#include "ochell.hh"

void check_error(cl_int err, const char *name) {
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")\n";
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	int len = 100;
	int *A = new int[len];
	int *B = new int[len];
	int *C = new int[len];
	
	for (int i = 0; i < len; ++i) {
		A[i] = 10 + i;
		B[i] = 100 + i;
	}

	// Get available platforms (implementations)
	std::vector<cl::Platform> platforms = get_platforms();
	
	if (platforms.size() == 0) {
		std::cerr << "ERROR: No platforms available\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "INFO: " << platforms.size() << " platforms available\n";
	for (size_t i = 0; i < platforms.size(); ++i)
		std::cout << "INFO: Platform " << i << " by: " << get_vendor(platforms[i]) << std::endl;

	cl_int err;
	// Create a context
	cl::Context context = create_context(platforms[0], CL_DEVICE_TYPE_CPU);

	// Alocate buffers for I/O
	cl::Buffer inA = create_buffer(context, "rh", len, A);
	cl::Buffer inB = create_buffer(context, "rh", len, B);
	cl::Buffer outC = create_buffer(context, "wh", len, C);
	
	// Get a device handler
	std::vector<cl::Device> devices = get_devices(context);
	std::cout << "INFO: " << devices.size() << " devices available\n";
	
	// Create the program
	cl::Program program(context, load_source("vector_add_kernel.cl"));
	err = program.build(devices, "");
	check_error(err, "Program::build()");
	
	// Create a kernel object for accessing kernel entry point
	cl::Kernel ker_vec_add(program, "vector_add", &err);
	check_error(err, "Kernel::Kernel()");
	// Set the arguments for this kernel (index, value)-pairs	
	set_kernel_args(ker_vec_add, inA, inB, outC, len);

	// Create a Command Queue for the device (1-to-1)
	cl::CommandQueue queue = create_command_queue(context, devices[0]);

	// Create an event to query the status of the execution	
	cl::Event event;
	// Execute the kernel
	err = queue.enqueueNDRangeKernel(
		ker_vec_add, cl::NullRange,
		cl::NDRange(len), cl::NDRange(1, 1), 0, &event
	);
	check_error(err, "CommandQueue::enqueueNDRangeKernel()");
	
	// Wait for the conclusion
	event.wait();
	blocking_read_buffer(queue, outC, 0, len, C);
	
	for (int i = 0; i < len; ++i)
		std::cout << C[i] << " ";
	std::cout << std::endl;
	
	exit(EXIT_SUCCESS);
}

