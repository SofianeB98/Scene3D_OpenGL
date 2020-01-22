#version 120

attribute vec2 a_position;
attribute vec2 a_texcoords;

varying vec2 v_position;
varying vec2 v_texcoords;

void main(void) 
{
	gl_Position = vec4(a_position, 0.0, 1.0); 

	v_texcoords = a_texcoords;
	v_position = a_position;
}