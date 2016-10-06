#version 400

layout(location=0) in vec3 vertex_position;
layout(location=1) in vec3 vertex_colour;

uniform mat4 matrixP;
uniform mat4 matrixV;
mat4 matrix;

out vec3 colour;

void main(){
	colour=vertex_colour;
	gl_PointSize=100.0f;
	matrix = matrixP * matrixV;
	gl_Position=matrix*vec4(vertex_position,1.0);
}