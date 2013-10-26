// Compiled with
// g++ -std=c++11 add_test.cpp -o add_test -l pocl && ./add_test
// or
// g++ -std=c++11 add_test.cpp -o add_test -l OpenCL && ./add_test

#include <CL/cl.hpp> // opencl.h on mac
#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <cerrno>

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
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	
	if (platforms.size() == 0) {
		std::cerr << "ERROR: No platforms available\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "INFO: " << platforms.size() << " platforms available\n";
	for (int i = 0; i < platforms.size(); ++i) {
		// Get platform vendor and print it
		std::string platform_vendor;
		platforms[i].getInfo(CL_PLATFORM_VENDOR, &platform_vendor);
		std::cout << "INFO: Platform " << i << " by: " << platform_vendor << std::endl;
	}
	cl_context_properties ctx_props[3] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0
	};
	
	cl_int err;
	// Create a context
	cl::Context context(CL_DEVICE_TYPE_CPU, ctx_props, 0, 0, &err);
	check_error(err, "Context::Context()");
	
	// Alocate buffers for I/O
	cl::Buffer inA(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, len, A, &err);
	check_error(err, "Buffer::Buffer()");
	cl::Buffer inB(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, len, B, &err);
	check_error(err, "Buffer::Buffer()");
	cl::Buffer outC(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, len, C, &err);
	check_error(err, "Buffer::Buffer()");
	
	// Get a device handler
	std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
	if (devices.size() == 0) {
		std::cerr << "ERROR: No devices available\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "INFO: " << devices.size() << " devices available\n";
	
	// Read the program source
	std::string program_str = read_file("vector_add_kernel.cl");
	const char *program_cstr = program_str.c_str();
	size_t program_str_len = program_str.size() + 1;
	// Create a program source
	cl::Program::Sources source(1,
		std::make_pair(program_cstr, program_str_len)
	);
	// Create the program
	cl::Program program(context, source);
	err = program.build(devices, "");
	check_error(err, "Program::build()");
	
	// Create a kernel object for accessing kernel entry point
	cl::Kernel ker_vec_add(program, "vector_add", &err);
	check_error(err, "Kernel::Kernel()");
	// Set the arguments for this kernel (index, value)-pairs	
	err = ker_vec_add.setArg(0, inA);
	err = ker_vec_add.setArg(1, inB);
	err = ker_vec_add.setArg(2, outC);
	err = ker_vec_add.setArg(3, len);
	
	// Create a Command Queue for the device (1-to-1)
	cl::CommandQueue queue(context, devices[0], 0, &err);
	check_error(err, "CommandQueue::CommandQueue()");

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
	err = queue.enqueueReadBuffer(outC, CL_TRUE, 0, len, C);
	check_error(err, "Queue::enqueueReadBuffer()");
	
	for (int i = 0; i < len; ++i)
		std::cout << C[i] << " ";
	std::cout << std::endl;
	
	exit(EXIT_SUCCESS);
}

