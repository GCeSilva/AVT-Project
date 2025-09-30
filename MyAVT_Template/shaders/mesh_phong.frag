#version 430

#define NUMBER_POINT_LIGHTS 6
#define NUMBER_SPOT_LIGHTS 2

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

in Data {
	vec3 normal;
	vec3 eye;
	vec2 tex_coord;
	vec3 pointLights[NUMBER_POINT_LIGHTS];
	vec3 spotLights[NUMBER_SPOT_LIGHTS];
} DataIn;

uniform Materials mat;

uniform sampler2D texmap;
uniform sampler2D texmap1;
uniform sampler2D texmap2;

uniform vec4 dirLight;

uniform int texMode;

uniform bool spotlight_mode;
uniform vec4 coneDir[NUMBER_SPOT_LIGHTS];
uniform float spotCosCutOff[NUMBER_SPOT_LIGHTS];

uniform bool pointLightMode;
uniform bool spotLightsOn;
uniform bool dirLightMode;
uniform bool fogMode;

out vec4 colorOut;

void main() {
	vec4 texel, texel1;

	vec4 spec = vec4(0.0);
	float intensity = 0.0f;
	float intSpec = 0.0f;

	float intensitySum = 0.0;
	vec4 specSum = vec4(0.0);

	float att = 0.0;
	float spotExp = 60.0;

	vec3 n = normalize(DataIn.normal);
	vec3 e = normalize(DataIn.eye);

	if(dirLightMode){
		intensity = max(dot(n, dirLight.xyz), 0.0);

		if (intensity > 0.0) {
			vec3 h = normalize(dirLight.xyz + e);
			intSpec = max(dot(h,n), 0.0);
			spec = mat.specular * pow(intSpec, mat.shininess);
		}

		intensitySum += intensity / NUMBER_POINT_LIGHTS;
		specSum += spec / NUMBER_POINT_LIGHTS;
	}

	if(pointLightMode){
		for(int i = 0; i < NUMBER_POINT_LIGHTS; i++){
			
			vec3 l = normalize(DataIn.pointLights[i]);

			intensity = max(dot(n,l), 0.0);

			if (intensity > 0.0) {
				vec3 h = normalize(l + e);
				intSpec = max(dot(h,n), 0.0);
				spec = mat.specular * pow(intSpec, mat.shininess);
			}

			intensitySum += intensity / NUMBER_POINT_LIGHTS;
			specSum += spec / NUMBER_POINT_LIGHTS;
		}
	}

	if(spotLightsOn){
		for(int i = 0; i < NUMBER_SPOT_LIGHTS; i++){
			
			vec3 ld = normalize(DataIn.spotLights[i]);

			vec3 sd = normalize(-coneDir[i].xyz);

			float spotCos = dot(ld, sd);

			if(spotCos > spotCosCutOff[i]){
				att = pow(spotCos, spotExp);
				intensity = max(dot(n,ld), 0.0);

				if (intensity > 0.0) {
					vec3 h = normalize(ld + e);
					intSpec = max(dot(h,n), 0.0);
					spec = mat.specular * pow(intSpec, mat.shininess) * att;
				}
				intensitySum += intensity / NUMBER_SPOT_LIGHTS;
				specSum += spec / NUMBER_SPOT_LIGHTS;
			}
		}
	}

	vec4 color = vec4(0.0);

	if(texMode == 0)	//no texturing
		color = vec4(max(intensitySum * mat.diffuse + specSum, mat.ambient).rgb, 1.0);

	else if(texMode == 1) // modulate diffuse color with texel color
	{
		texel = texture(texmap2, DataIn.tex_coord);  // texel from lighwood.tga
		color = vec4(max(intensitySum * mat.diffuse * texel + specSum, 0.07 * texel).rgb, 1.0);
	}
	else if (texMode == 2) // diffuse color is replaced by texel color
	{
		texel = texture(texmap, DataIn.tex_coord);  // texel from stone.tga
		color = vec4(max(intensitySum * texel + specSum, 0.07 * texel).rgb, 1.0);
	}
	else // multitexturing
	{
		texel = texture(texmap2, DataIn.tex_coord);  // texel from lighwood.tga
		texel1 = texture(texmap1, DataIn.tex_coord);  // texel from checker.tga
		color = vec4(max(intensitySum * texel * texel1 + specSum, 0.07 * texel *texel1).rgb, 1.0);
	}
	
	if(fogMode){
		float dist = length(-DataIn.eye);
		float fogAmount = exp(-dist*0.1);
		vec3 fogColor = vec3(0.5,0.6,0.7);
		color = vec4(mix(fogColor, color.rgb, fogAmount), 1.0);
	}

	colorOut = color;
	
}