#ifndef __OCHELL_H__
#define __OCHELL_H__

/////////////////////////////////
//  Help functions for OpenCL  //
// by  Alessandro "AkiRoss" Re //
/////////////////////////////////

/* This module should help in these phases:

V  1. Get info about platform
V  2. Get devices and set which to use
V  3. Create a context
V  4. Create a command queue
V  5. Create buffer objects
   7. Transfer data
V  8. Create program
   9. Link the program
  10. Create kernel objects
V 11. Set arguments
  12. Execute kernel
V 13. Read memory objects

Feature:
 - Easy and quick to use
 - Automatic error reporting (TODO should have enabling flag)
*/

#include <cerrno>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#ifndef __APPLE__
	#include <CL/cl.h>
#else
	#include <OpenCL/opencl.h>
#endif

struct OCHException;

// Basic functions, to acess "raw" functionalites

// Read a whole file inside a std::string
std::string read_file(const std::string &path);

// Get the list of platforms present
std::vector<cl::Platform> get_platforms();

// Get the vendor of a given platform
std::string get_vendor(const cl::Platform &pl);

// Create a context using the first platform available
cl::Context create_context(cl_device_type type=CL_DEVICE_TYPE_ALL);

// Create context for the specified platform
cl::Context create_context(cl::Platform &platform,
	cl_device_type type=CL_DEVICE_TYPE_ALL);
	
// Get devices usable from a context
std::vector<cl::Device> get_devices(const cl::Context &ctx);

// Get devices usable from a platform
std::vector<cl::Device> get_devices(const cl::Platform &platform,
	cl_device_type type=CL_DEVICE_TYPE_ALL);
	
// Loads a single source file
cl::Program::Sources load_source(const std::string &path);

// Loads source files in an iterator
template <class Iter>
cl::Program::Sources load_sources(Iter paths_begin, Iter paths_end);

// Set arguments to a kernel object
template <typename... Args>
void set_kernel_args(cl::Kernel &kernel, Args... values);

// Create a buffer object using the specified flags, size and data
cl::Buffer create_buffer(cl::Context &context, const std::string &flags,
	size_t size, void *host_ptr=NULL);

// Create a command queue for the given context and device
cl::CommandQueue create_command_queue(cl::Context &context, cl::Device &device,
	cl_command_queue_properties properties = 0);

// Read size bytes starting at offset from the buffer, and store it in ptr
void blocking_read_buffer(cl::CommandQueue &queue, cl::Buffer &buffer,
	size_t offset, size_t size, void *ptr);

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// Implementation /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct OCHException {
	OCHException(const std::string &w, cl_int e):
		message(w), error(e) {
	}
	std::string what() const {
		std::stringstream ss;
		ss << message << " (" << error << ")";
		return ss.str();
	}
	const std::string message;
	const cl_int error;
};

std::string read_file(const std::string &path) {
	std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

std::vector<cl::Platform> get_platforms() {
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	return platforms;
}

std::string get_vendor(const cl::Platform &pl) {
	std::string vendor;
	pl.getInfo(CL_PLATFORM_VENDOR, &vendor);
	return vendor;
}

cl::Context create_context(cl_device_type type) {
	std::vector<cl::Platform> platforms = get_platforms();
	if (platforms.size() == 0)
		throw OCHException("create_context() no platforms available", 0);
	return create_context(platforms[0], type);
}

cl::Context create_context(cl::Platform &platform, cl_device_type type) {
	cl_context_properties props[3] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(), 0
	};
	cl_int error;
	cl::Context context(type, props, 0, 0, &error);
	if (error != CL_SUCCESS)
		throw OCHException("Context::Context()", error);
	return context;
}

std::vector<cl::Device> get_devices(const cl::Context &ctx) {
	return ctx.getInfo<CL_CONTEXT_DEVICES>();
}

std::vector<cl::Device> get_devices(const cl::Platform &platform,
	cl_device_type type)
{
	std::vector<cl::Device> devices;
	cl_int error = platform.getDevices(type, &devices);
	if (error != CL_SUCCESS)
		throw OCHException("Platform::getDevices()", error);
	return devices;
}

cl::Program::Sources load_source(const std::string &path) {
	std::string src_str = read_file(path);
	const char *src_cstr = src_str.c_str();
	size_t src_len = src_str.size() + 1;
	// Sources is a vector
	cl::Program::Sources source(1, std::make_pair(src_cstr, src_len));
	return source;
}

template <class Iter>
cl::Program::Sources load_sources(Iter paths_begin, Iter paths_end) {
	cl::Program::Sources sources; // Vector class
	while (paths_begin != paths_end) {
		std::string src_str = read_file(*paths_begin);
		const char *src_cstr = src_str.c_str();
		size_t src_len = src_str.size() + 1;
		sources.push_back(std::make_pair(src_cstr, src_len));
		paths_begin++;
	}
	return sources;
}

template <class Tp>
void set_kernel_args(cl::Kernel &kernel, int pos, int count, Tp value) {
	if (count > 0) {
		cl_int error = kernel.setArg(pos, value);
		if (error != CL_SUCCESS)
			throw OCHException("Kernel::setArg()", error);
	}
}

// Private implementation, do not use this
template <class Tp, typename... Args>
void set_kernel_args(cl::Kernel &kernel, int pos, int count, Tp value,
	Args... values)
{
	if (count > 0) {
		cl_int error = kernel.setArg(pos, value);
		if (error != CL_SUCCESS)
			throw OCHException("Kernel::setArg()", error);
		set_kernel_args(kernel, pos + 1, count - 1, values...);
	}
}

// This is the function version to use
template <typename... Args>
void set_kernel_args(cl::Kernel &kernel, Args... values) {
	static const int size = sizeof...(Args);
	set_kernel_args(kernel, 0, size, values...);
}

cl::Buffer create_buffer(cl::Context &context, const std::string &flag_str,
	size_t size, void *host_ptr)
{
	cl_mem_flags flags = 0;
	for (size_t i = 0; i < flag_str.size(); ++i) {
		switch (flag_str[i]) {
			case 'r':
				if (flags & CL_MEM_WRITE_ONLY) {
					flags = flags ^ CL_MEM_WRITE_ONLY; // Disable write
					flags = flags | CL_MEM_READ_WRITE; // Enable read-write
				}
				else
					flags = flags | CL_MEM_READ_ONLY; // Enable only read
				break;
			case 'w':
				if (flags & CL_MEM_READ_ONLY) {
					flags = flags ^ CL_MEM_READ_ONLY; // Disable write
					flags = flags | CL_MEM_READ_WRITE; // Enable read-write
				}
				else
					flags = flags | CL_MEM_WRITE_ONLY; // Enable only read
				break;
			case 'h':
				flags = flags | CL_MEM_USE_HOST_PTR;
				break;
			case 'a':
				flags = flags | CL_MEM_ALLOC_HOST_PTR;
				break;
			case 'c':
				flags = flags | CL_MEM_COPY_HOST_PTR;
				break;
			default:
				throw OCHException("create_buffer() invalid flag", flag_str[i]);
		}
	}
	cl_int error;
	cl::Buffer buff(context, flags, size, host_ptr, &error);
	if (error != CL_SUCCESS)
		throw OCHException("Buffer::Buffer()", error);
	return buff;
}

cl::CommandQueue create_command_queue(cl::Context &context, cl::Device &device,
	cl_command_queue_properties properties)
{
	cl_int error;
	cl::CommandQueue queue(context, device, properties, &error);
	if (error != CL_SUCCESS)
		throw OCHException("CommandQueue::CommandQueue()", error);
	return queue;
}

void blocking_read_buffer(cl::CommandQueue &queue, cl::Buffer &buffer,
	size_t offset, size_t size, void *ptr)
{
	cl_int error = queue.enqueueReadBuffer(buffer, CL_TRUE, offset, size, ptr);
	if (error != CL_SUCCESS)
		throw OCHException("Queue::enqueueReadBuffer()", error);
}

#endif /* __OCHELL_H__ */

