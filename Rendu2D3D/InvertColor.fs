#version 120

varying vec2 v_position;
varying vec2 v_texcoords;

uniform sampler2D u_sampler;



void main(void) 
{
	vec4 color = texture2D(u_sampler, v_texcoords);

	gl_FragColor = 1 - color;
}
