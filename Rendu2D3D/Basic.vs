attribute vec3 a_position;
attribute vec3 a_normals;
attribute vec3 a_color;
attribute vec2 a_texcoords;

//uniform mat4 u_matriceRot;
//uniform mat4 u_matriceScale;
//uniform mat4 u_matriceTranslation;
uniform mat4 u_matriceProjection;
uniform mat4 u_worldMatrix;
uniform mat4 u_view;

varying vec3 v_color;
varying vec3 v_position;
varying vec3 v_normals;
varying vec2 v_texcoords;


void main(void) 
{ 
	// on passe les coord. de texture directement rasterizer (et donc fragment)
	v_texcoords = a_texcoords;
	v_normals = mat3(transpose(inverse(u_worldMatrix))) * a_normals;
	
	//u_matriceTranslation * u_matriceRot * u_matriceScale
	
	gl_Position = u_matriceProjection * u_view * u_worldMatrix * vec4(a_position, 1.0);
	
	v_position = vec3(u_worldMatrix * vec4(a_position, 1.0));
	
	v_color = a_color; 
}