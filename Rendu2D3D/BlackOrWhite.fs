#version 120

varying vec2 v_position;
varying vec2 v_texcoords;

uniform sampler2D u_sampler;

float getAverage(vec3 color)
{
	return (color.r + color.g + color. b) / 3;
}

void main(void) 
{
	vec4 color = texture2D(u_sampler, v_texcoords);

	float avarage = getAverage(color.rgb);

	gl_FragColor = avarage > 0.5? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1);	
}
