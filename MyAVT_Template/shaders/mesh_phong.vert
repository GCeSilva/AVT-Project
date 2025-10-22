#version 430

#define NUMBER_POINT_LIGHTS 6
#define NUMBER_SPOT_LIGHTS 2

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat4 m_model;
uniform mat3 m_normal;

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;
in vec4 tangent;

uniform int texMode;
uniform vec4 dirLight;
uniform vec4 pointLights[NUMBER_POINT_LIGHTS];
uniform vec4 spotLights[NUMBER_SPOT_LIGHTS];

out Data {
	vec3 normal;
	vec3 eye;
	vec3 eye_default;
	vec2 tex_coord;
	vec3 dirLight;
	vec3 pointLights[NUMBER_POINT_LIGHTS];
	vec3 spotLights[NUMBER_SPOT_LIGHTS];
	vec3 skyboxTexCoord;
} DataOut;

uniform bool bumpMapMode;

void main () {

	vec4 pos = m_viewModel * position;

	DataOut.normal = normalize(m_normal * normal.xyz);

	DataOut.dirLight = vec3(dirLight);
	for(int i = 0; i < NUMBER_POINT_LIGHTS; i++) {
		DataOut.pointLights[i] = vec3(pointLights[i] - pos);
	}
	for(int i = 0; i < NUMBER_SPOT_LIGHTS; i++) {
		DataOut.spotLights[i] = vec3(spotLights[i] - pos);
	}

	DataOut.eye = vec3(-pos);
	DataOut.eye_default = vec3(-pos);

	DataOut.skyboxTexCoord = vec3(m_model * position);	//Transformação de modelação do cubo unitário 
	DataOut.skyboxTexCoord.x = - DataOut.skyboxTexCoord.x; //Texturas mapeadas no interior logo negar a coordenada x
	DataOut.tex_coord = texCoord.st;

	vec3 n, t, b;
	vec3 aux;
	n = DataOut.normal;
	if (texMode == 2 && bumpMapMode)
	{
		//Calculate components of TBN basis in eye space
		t = normalize(m_normal * tangent.xyz);  
		b = tangent.w * cross(n,t);

		aux.x = dot(DataOut.dirLight, t);
		aux.y = dot(DataOut.dirLight, b);
		aux.z = dot(DataOut.dirLight, n);
		DataOut.dirLight = normalize(aux);

		for (int i = 0; i < NUMBER_POINT_LIGHTS; i++) {
			aux.x = dot(DataOut.pointLights[i], t);
			aux.y = dot(DataOut.pointLights[i], b);
			aux.z = dot(DataOut.pointLights[i], n);
			DataOut.pointLights[i] = normalize(aux);
		}
		for (int i = 0; i < NUMBER_SPOT_LIGHTS; i++) {
			aux.x = dot(DataOut.spotLights[i], t);
			aux.y = dot(DataOut.spotLights[i], b);
			aux.z = dot(DataOut.spotLights[i], n);
			DataOut.spotLights[i] = normalize(aux);
		}

		aux.x = dot(DataOut.eye, t);
		aux.y = dot(DataOut.eye, b);
		aux.z = dot(DataOut.eye, n);
		DataOut.eye = normalize(aux);
	}

	gl_Position = m_pvm * position;	
}