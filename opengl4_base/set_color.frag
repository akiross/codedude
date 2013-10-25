#version 420 core

out vec4 fColor;

// Uses a uniform variable to set the color
uniform vec4 MyColor;

void main() {
	fColor = MyColor;
}
