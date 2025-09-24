#version 430

#define NUMBER_POINT_LIGHTS 6
#define NUMBER_SPOT_LIGHTS 2

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;

uniform vec4 pointLights[NUMBER_POINT_LIGHTS];
uniform vec4 spotLights[NUMBER_SPOT_LIGHTS];

out Data {
	vec3 normal;
	vec3 eye;
	vec2 tex_coord;
	vec3 pointLights[NUMBER_POINT_LIGHTS];
	vec3 spotLights[NUMBER_SPOT_LIGHTS];
} DataOut;

void main () {

	vec4 pos = m_viewModel * position;

	DataOut.normal = normalize(m_normal * normal.xyz);

	for(int i = 0; i < NUMBER_POINT_LIGHTS; i++) {
		DataOut.pointLights[i] = vec3(pointLights[i] - pos);
	}
	for(int i = 0; i < NUMBER_SPOT_LIGHTS; i++) {
		DataOut.spotLights[i] = vec3(spotLights[i] - pos);
	}

	DataOut.eye = vec3(-pos);
	DataOut.tex_coord = texCoord.st;

	gl_Position = m_pvm * position;	
}