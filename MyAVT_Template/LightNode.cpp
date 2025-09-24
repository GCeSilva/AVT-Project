#include "LightNode.h"

int PointLightNode::shaderArrayIndexCount = 0;
int SpotLightNode::shaderArrayIndexCount = 0;

//THESE METHODS DO NOT CARE FOR SHADER ARRAY SIZE SO, WHEN USED, MAKE SURE ITS WITHIN THE BOUNDS
void PointLightNode::CalculateLight(int* shaderArrayIndex) {

	if (parent) {
		float tempPointLight[4] = {};

		//i wish there were a better way to do this
		tempPointLight[0] = (*parent->localTransform.translation)[0] + position[0];
		tempPointLight[1] = (*parent->localTransform.translation)[0] + position[1];
		tempPointLight[2] = (*parent->localTransform.translation)[0] + position[2];
		tempPointLight[3] = (*parent->localTransform.translation)[0] + position[3];

		renderer.SetPointLights(&tempPointLight, shaderArrayIndex);
	}
	else renderer.SetPointLights(&position, shaderArrayIndex);
}

void DirectionalLightNode::CalculateLight(int* shaderArrayIndex) {
	renderer.SetDirectionalLight(position);
}

//THESE METHODS DO NOT CARE FOR SHADER ARRAY SIZE SO, WHEN USED, MAKE SURE ITS WITHIN THE BOUNDS
void SpotLightNode::CalculateLight(int* shaderArrayIndex) {

		if (parent) {

			float tempSpotLight[4] = {};
			float tempConeDir[4] = {};

			tempSpotLight[0] = position[0] + (*parent->localTransform.translation)[0];
			tempSpotLight[1] = position[1] + (*parent->localTransform.translation)[1];
			tempSpotLight[2] = position[2] + (*parent->localTransform.translation)[2];
			tempSpotLight[3] = position[3];

			// later turn this into proper spherical coordinates
			// also these need to be in degrees, hence the division by (PI/180)
			tempConeDir[0] = coneDirection[0] + cos(parent->axisRotations[1]) / (PI / 180.0f);
			tempConeDir[1] = coneDirection[1];
			tempConeDir[2] = coneDirection[2] + sin(parent->axisRotations[1]) / (PI / 180.0f);
			tempConeDir[3] = coneDirection[3];

			renderer.SetSpotLights(&tempSpotLight, shaderArrayIndex);
			renderer.setSpotParam(&tempConeDir, cutOff, shaderArrayIndex);
		}
		else {
			renderer.SetSpotLights(&position, shaderArrayIndex);
			renderer.setSpotParam(&coneDirection, cutOff, shaderArrayIndex);
		}	
}