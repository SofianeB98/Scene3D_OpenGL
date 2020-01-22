#version 120

varying vec2 v_position;
varying vec2 v_texcoords;

uniform sampler2D u_sampler;

vec3 sepia(vec3 color)
{
	vec3 sep = vec3
	(
	dot(color.rgb, vec3(0.393, 0.769, 0.189)),
	dot(color.rgb, vec3(0.349, 0.686, 0.168)),
	dot(color.rgb, vec3(0.272, 0.534, 0.131))
	);

return sep;
}

void main(void) 
{
	vec4 color = texture2D(u_sampler, v_texcoords);

	vec3 sep = sepia(color.rgb);


	gl_FragColor = vec4(sep, 1.0);	
}
