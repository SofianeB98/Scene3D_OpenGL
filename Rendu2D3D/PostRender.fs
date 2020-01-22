#version 120

varying vec2 v_position;
varying vec2 v_texcoords;

uniform sampler2D u_sampler;

float luminance(vec3 color)
{
	const vec3 lumWeight = vec3(0.2125, 0.7154, 0.072);
	return dot(color.rgb, lumWeight);
}

void main(void) 
{
	vec4 color = texture2D(u_sampler, v_texcoords);

	float lum = luminance(color.rgb);

	vec3 luminanceVec = vec3(lum);

	gl_FragColor = vec4(luminanceVec, 1.0);	
}
