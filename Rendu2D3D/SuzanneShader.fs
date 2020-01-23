struct PointLight
{
	vec3 lightPosition;

	vec3 lightDiffuse;
	vec3 lightSpecular;
	vec3 lightAmbient;

	//Attenuation
	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight
{
	vec3 lightDirection;

	vec3 lightDiffuse;
	vec3 lightSpecular;
	vec3 lightAmbient;
};

struct SpotLight
{
	vec3 lightDirection;
	vec3 lightPosition;

	vec3 lightDiffuse;
	vec3 lightSpecular;
	vec3 lightAmbient;

	float cutoff;
	float outerCutoff;
};

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material u_material;
uniform PointLight u_light;
uniform DirectionalLight u_dLight;
uniform SpotLight u_sLight;
uniform vec3 u_skyColor;
uniform vec3 u_groundColor;
uniform vec3 u_viewPos;

varying vec3 v_color;
varying vec3 v_position;
varying vec3 v_normals;
varying vec2 v_texcoords;

float gamma = 2.2;

//Utils function
vec3 ambientLight(vec3 str, vec3 color, vec3 norm)
{
	vec3 up = str;
	up.x = 0;
	up.y = -1;
	up.z = 0;
	float hemisphere = dot(up, norm) * 0.5 + 0.5;

	vec3 amb = str * color;
	amb = amb * mix(u_skyColor, u_groundColor, hemisphere);

	return pow(amb, vec3(gamma));
}

vec3 diffuseLight(vec3 norm, vec3 lightDir, vec3 lColor, vec3 mColor)
{
	float diffuse = max(dot(norm, lightDir), 0.0);
	vec3 dLight = lColor * (diffuse * mColor);

	return dLight;
}

vec3 specularLight(vec3 norm, vec3 viewDir, vec3 lightDir, vec3 lColor, vec3 mColor, float mShine)
{
	vec3 h = normalize((lightDir + viewDir));
	vec3 spec = max(pow(dot(norm, h), mShine), 0) * lColor * mColor;

	return spec;
}

vec3 CalculatePointLight(PointLight pLight, vec3 norm, vec3 vPosition, vec3 viewDir)
{
	vec3 lightDir = normalize(pLight.lightPosition - vPosition);

	float distance = length(pLight.lightPosition - v_position);
	float attenuation = 1.0/ (pLight.constant + pLight.linear * distance + pLight.quadratic *  (distance * distance));

	//Ambiant
	vec3 ambient = ambientLight(pLight.lightAmbient, u_material.ambient, norm);
	ambient *= attenuation;

	//Diffuse
	vec3 diffuse = diffuseLight(norm, lightDir, pLight.lightDiffuse, u_material.diffuse);
	diffuse *= attenuation;

	//Specular
	vec3 spec = specularLight(norm, viewDir, lightDir, pLight.lightSpecular, u_material.specular, u_material.shininess);
	spec *= attenuation;

	return (ambient + diffuse + spec);
}

vec3 CalculateDirLight(DirectionalLight dLight, vec3 norm, vec3 viewDir)
{
	vec3 lightDir = normalize(-dLight.lightDirection);

	//Ambiant
	vec3 ambient = ambientLight(dLight.lightAmbient, u_material.ambient, norm);

	//Diffuse
	vec3 diffuse = diffuseLight(norm, lightDir, dLight.lightDiffuse, u_material.diffuse);

	//Specular
	vec3 spec = specularLight(norm, viewDir, lightDir, dLight.lightSpecular, u_material.specular, u_material.shininess);

	return (ambient + diffuse + spec);
}

vec3 CalculateSpotLight(SpotLight sLight, vec3 norm, vec3 vPosition, vec3 viewDir)
{
	vec3 lightDir = normalize(sLight.lightPosition - vPosition);

	//Ambiant
	vec3 ambient = ambientLight(sLight.lightAmbient, u_material.ambient, norm);

	float theta = dot(lightDir, normalize(-sLight.lightDirection));
	float epsi = sLight.cutoff - sLight.outerCutoff;
	float intensity = clamp((theta - sLight.outerCutoff) / epsi, 0.0, 1.0);

	vec3 result = ambient;
	//Diffuse
	result += diffuseLight(norm, lightDir, sLight.lightDiffuse, u_material.diffuse) * intensity;
	//Specular
	result += specularLight(norm, viewDir, lightDir, sLight.lightSpecular, u_material.specular, u_material.shininess) * intensity;

	return result;
}

//Main boucle
void main(void) 
{
	vec3 norm = normalize(v_normals);
	vec3 viewDir = normalize(u_viewPos - v_position);
	
	vec3 pointLight = CalculatePointLight(u_light, norm, v_position, viewDir);

	vec3 directionalLight = CalculateDirLight(u_dLight, norm, viewDir);

	vec3 spotLight = CalculateSpotLight(u_sLight, norm, v_position, viewDir);

	vec3 result = directionalLight + pointLight + spotLight;

	//Application au vertex
	gl_FragColor = vec4(pow(result, vec3(1.0/gamma)), 1.0);	
}