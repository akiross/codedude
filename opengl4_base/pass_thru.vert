#version 420 core

layout(location = 0) in vec4 vPosition;

void main() {
	gl_Position = vPosition;// + vec4(0.1, 0.1, 0.1, 1.0);
}
