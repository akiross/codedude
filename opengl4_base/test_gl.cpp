// Compile depending on the version
// g++ test_gl.cpp gl_core_3_1.c -o test_gl -lglut -lGL -lGLU
// g++ test_gl.cpp gl_core_4_2.c -o test_gl -lglut -lGL -lGLU

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <string>
#include <cerrno>
#include <vector>
//#include <GL/glew.h> // Crashes the program...
// Using glLoadGen instead
//#include "gl_core_3_1.h" // OpenGL 3.1
#include "gl_core_4_2.h" // OpenGL 4.2
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
 
struct Scene {
	int win_w, win_h;

	float aspect() { return win_w / win_h; }
	Scene() {
		win_w = win_h = 400;
	}
} scene;

// Store informations about shader source codes
struct ShaderInfo {
	GLenum type; // Shader type (vertex, fragment...)
	const char *filename; // File path
};

enum { Triangles, NUM_VAO }; // Vertex Array Objects used
enum { ArrayBuffer, NUM_BUFFER }; // Buffer Objects used 
enum { vPosition = 0 }; // Shaders attributes

GLuint vaos[NUM_VAO]; // Storage for VAO names
GLuint buffers[NUM_BUFFER]; // Storage for VBO names

const GLuint NUM_VERTICES = 6;

// Read a whole file inside a std::string
std::string read_file(const char *path) {
	std::ifstream in(path, std::ios::in | std::ios::binary);
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

GLuint load_shaders(const ShaderInfo *shaders) {
	GLuint program = glCreateProgram();
	std::vector<GLuint> shader_objects;
	for (int i = 0; shaders[i].type != GL_NONE; ++i) {
		// Read the source file
		std::string code = read_file(shaders[i].filename);
		const char *c_code = code.c_str();
		// Allocate a shader object
		GLuint shader = glCreateShader(shaders[i].type);
		// Associate soure to a shader (1 string in &c_code array, null term)
		glShaderSource(shader, 1, &c_code, 0);
		// Compile the shader
		glCompileShader(shader);
		// Check compilation outcome
		GLint compile_status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
		if (compile_status != GL_TRUE) {
			GLint buff_size, log_size;
			// Get log size and use it for creating a buffer
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &buff_size);
			char *log = new char[buff_size];
			// Copy the log in the buffer
			glGetShaderInfoLog(shader, buff_size, &log_size, log);
			std::cerr << "ERROR (Shader Compilation): " << shaders[i].filename << ": " << log << std::endl;
			delete[] log;
		}
		else {		
			std::cout << "INFO: Shader " << shaders[i].filename << " loaded.\n";
			// Attach the shader to the program
			glAttachShader(program, shader);
			// Save this object
			shader_objects.push_back(shader);
		}
	}
	// Link the shaders into the program
	glLinkProgram(program);
	// Check for linking status
	GLint link_status;
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		GLint buff_size, log_size;
		// Get length of the buffer and create it
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buff_size);
		char *log = new char[buff_size];
		// Get log
		glGetProgramInfoLog(program, buff_size, &log_size, log);
		std::cerr << "ERROR (Program Linking): " << log << std::endl;
		delete[] log;
	}
	// Clear the shaders info, not required anymore
	for (int i = 0; i < shader_objects.size(); ++i)
		glDeleteShader(shader_objects[i]);
	return program;
}

void ogl_check_error(const char *msg="") {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		std::cerr << "ERROR: " << gluErrorString(error) << " (" << msg << ")\n";
}

void ogl_init() {
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	// Setup the background color when clearing the window
	glClearColor(1.0, 0.0, 0.0, 0.0);
	
	// Generate names for NUM_VAO vertex-array objects
	glGenVertexArrays(NUM_VAO, vaos);
	// Allocate and bind the VAO specified by the name
	glBindVertexArray(vaos[Triangles]);

	ogl_check_error();
	
	GLfloat vertices[NUM_VERTICES][2] = {
		{ -0.90, -0.90 },
		{  0.85, -0.90 },
		{ -0.90,  0.85 },
		{  0.90, -0.85 },
		{  0.90,  0.90 },
		{ -0.85,  0.90 }
	};

	// Generate names for NUM_BUFFER buffer objects
	glGenBuffers(NUM_BUFFER, buffers);
	// Allocate space for a buffer object and make it current
	glBindBuffer(GL_ARRAY_BUFFER, buffers[ArrayBuffer]);
	// Load some data in the buffer object, the data will not change
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	ogl_check_error();
	
	// Setup shaders informations
	ShaderInfo shaders[] = {
		{GL_VERTEX_SHADER, "pass_thru.vert"},
		{GL_FRAGMENT_SHADER, "set_color.frag"},
		{GL_NONE, NULL}
	};
	
	// Load shaders
	GLuint program = load_shaders(shaders);
	// Set the shader program to use
	glUseProgram(program);
	
	// Specify where data for index (shader attribute location) can be accessed
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// Enable the usage of the attribute with given index
	glEnableVertexAttribArray(vPosition);
}

void ogl_resize(int w, int h) {
	scene.win_w = w;
	scene.win_h = h;
    glViewport(0, 0, w, h);
}

void ogl_display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ogl_check_error("After cleared");

	// Chose vertex array to use
	glBindVertexArray(vaos[Triangles]);
	// Draw the geometry
	glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);

	glutSwapBuffers();
	ogl_check_error("After swapped");
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	// Ask for a context of version Major.Minor
	glutInitContextVersion(4, 2);
	// Ask for the core profile
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// Forward compatible is not really necessary with core profile
//	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	// Initialize glut toolkit
	glutInit(&argc, argv);
	// Close window and return mainloop when close button is pressed
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	// Set window size
	glutInitWindowSize(scene.win_w, scene.win_h);
	// Ask for depth buffer, double buffering and RGBA colors
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	// Create the window
	if (glutCreateWindow("OpenGL test") < 1) {
		std::cerr << "ERROR: Could not create a window.\n";
		exit(EXIT_FAILURE);
	}
	
	// glew crashes the program on my machine
//	glewExperimental = GL_FALSE;
//	if (glewInit() != GLEW_OK) {
//		std::cerr << "ERROR: Could not initialize GLEW\n";
//		exit(EXIT_FAILURE);
//	}

	// Using opengl loader instead
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		std::cerr << "ERROR: Could not initialize GLEW\n";
		exit(EXIT_FAILURE);
	}
	// GLEW may create an error, but it can be ignored
//	ogl_check_error("Ignored");
	// Set callbacks
	glutReshapeFunc(ogl_resize);
	glutDisplayFunc(ogl_display);
	// Initialize OpenGL
	ogl_init();
	// Start!
	glutMainLoop();
	exit(EXIT_SUCCESS);
}
