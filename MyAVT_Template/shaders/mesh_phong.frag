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
	vec3 eye_default;
	vec2 tex_coord;
	vec3 dirLight;
	vec3 pointLights[NUMBER_POINT_LIGHTS];
	vec3 spotLights[NUMBER_SPOT_LIGHTS];
	vec3 skyboxTexCoord;
} DataIn;

uniform Materials mat;

uniform sampler2D texmap;
uniform sampler2D texmap1;
uniform sampler2D texmap2;
uniform sampler2D texmap3;

//assimp
uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D normalTex;
//tree sprite
uniform sampler2D treeTex;
// stone normal
uniform sampler2D stoneNormalTex;
// skybox
uniform samplerCube cubeMap;

uniform int texMode;

uniform bool spotlight_mode;
uniform vec4 coneDir[NUMBER_SPOT_LIGHTS];
uniform float spotCosCutOff[NUMBER_SPOT_LIGHTS];

uniform bool pointLightMode;
uniform bool spotLightsOn;
uniform bool dirLightMode;
uniform bool fogMode;
uniform bool bumpMapMode;

out vec4 colorOut;

void main() {

	vec4 texel, texel1, texelDif, texelSpec, texelNorm, texelTree;

	vec4 spec = vec4(0.0);
	float intensity = 0.0f;
	float intSpec = 0.0f;

	float intensitySum = 0.0;
	vec4 specSum = vec4(0.0);

	float att = 0.0;
	float spotExp = 60.0;

	vec3 n;
	if(texMode == 2 && bumpMapMode)  // lookup normal from normal map, move from [0,1] to [-1, 1] range, normalize
		n = normalize(2.0 * texture(stoneNormalTex, DataIn.tex_coord).rgb - 1.0);
	else
		n = normalize(DataIn.normal);
	vec3 e = normalize(DataIn.eye);

	if(dirLightMode){
		intensity = max(dot(n, DataIn.dirLight), 0.0);

		if (intensity > 0.0) {
			vec3 h = normalize(DataIn.dirLight + e);
			intSpec = max(dot(h,n), 0.0);
			spec = mat.specular * pow(intSpec, mat.shininess);
		}

		intensitySum += intensity;
		specSum += spec;
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

			intensitySum += intensity / 3;
			specSum += spec / 3;
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
				intensitySum += intensity;
				specSum += spec;
			}
		}
	}
	

	vec4 color = vec4(0.0);

	if(texMode == 0)	//no texturing
		color = vec4(max(intensitySum * mat.diffuse + specSum, mat.ambient).rgb, mat.diffuse.a);

	else if(texMode == 1) // modulate diffuse color with texel color
	{
		texel = texture(texmap2, DataIn.tex_coord);  // texel from lighwood.tga
		color = vec4(max(intensitySum * mat.diffuse * texel + specSum, 0.07 * texel).rgb, mat.diffuse.a);
	}
	else if (texMode == 2) // diffuse color is replaced by texel color
	{
		texel = texture(texmap, DataIn.tex_coord);  // texel from stone.tga
		color = vec4(max(intensitySum * texel + specSum, 0.07 * texel).rgb, mat.diffuse.a);
	}
	else if (texMode == 3)
	{
		texel = texture(texmap3, DataIn.tex_coord);  // texel from mosaic.tga
		color = vec4(max(intensitySum * texel + specSum, 0.07 * texel).rgb, mat.diffuse.a);
	}
	else if (texMode == 4)
	{
		texelDif = texture(diffuseTex, DataIn.tex_coord);
		texelSpec = texture(specularTex, DataIn.tex_coord);

		color = vec4(max(intensitySum * texelDif * texelSpec, 0.07 * texelDif * texelSpec).rgb, mat.diffuse.a);
	}
	else if (texMode == 5){
		texelTree = texture(treeTex, DataIn.tex_coord);
		if(texelTree.a == 0.0){
			discard;
		}
		else{
			color = vec4(max(intensitySum * texelTree + specSum, 0.07 * texelTree).rgb, texelTree.a);
		}
	}
	else if (texMode == 9){
		color = texture(cubeMap, DataIn.skyboxTexCoord);
	}
	else // multitexturing
	{
		texel = texture(texmap2, DataIn.tex_coord);  // texel from lighwood.tga
		texel1 = texture(texmap1, DataIn.tex_coord);  // texel from checker.tga
		color = vec4(max(intensitySum * texel * texel1 + specSum, 0.07 * texel *texel1).rgb, mat.diffuse.a);
	}
	
	if(fogMode){
		float dist = length(-DataIn.eye_default);
		float fogAmount = exp(-dist*0.02);
		vec3 fogColor = vec3(0.5,0.6,0.7);
		color = vec4(mix(fogColor, color.rgb, fogAmount), mat.diffuse.a);
	}

	colorOut = color;
	
}