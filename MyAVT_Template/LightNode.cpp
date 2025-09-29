#include "LightNode.h"

int PointLightNode::shaderArrayIndexCount = 0;
int SpotLightNode::shaderArrayIndexCount = 0;

//THESE METHODS DO NOT CARE FOR SHADER ARRAY SIZE SO, WHEN USED, MAKE SURE ITS WITHIN THE BOUNDS
void PointLightNode::CalculateLight(int* shaderArrayIndex) {

	if (parent) {
		float tempPointLight[4] = {};

		Node* tempParent = parent;
		while (tempParent != nullptr) {
			tempPointLight[0] += (*tempParent->localTransform.translation)[0];
			tempPointLight[1] += (*tempParent->localTransform.translation)[1];
			tempPointLight[2] += (*tempParent->localTransform.translation)[2];

			tempParent = tempParent->GetParent();
		}

		//i wish there were a better way to do this
		tempPointLight[0] += position[0];
		tempPointLight[1] += position[1];
		tempPointLight[2] += position[2];
		tempPointLight[3] = position[3];

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

			float tempAxisRot[3] = {};

			//full parent inheritance
			Node* tempParent = parent;
			while (tempParent != nullptr) {
				tempSpotLight[0] += (*tempParent->localTransform.translation)[0];
				tempSpotLight[1] += (*tempParent->localTransform.translation)[1];
				tempSpotLight[2] += (*tempParent->localTransform.translation)[2];

				tempAxisRot[0] = tempParent->axisRotations[0];
				tempAxisRot[1] = tempParent->axisRotations[1];
				tempAxisRot[2] = tempParent->axisRotations[2];

				tempParent = tempParent->GetParent();
			}


			tempSpotLight[0] += position[0];
			tempSpotLight[1] += position[1];
			tempSpotLight[2] += position[2];
			tempSpotLight[3] = position[3];

			// later turn this into proper spherical coordinates
			// also these need to be in degrees, hence the division by (PI/180)
			// these have to be turned into degrees because these are not going to be processed by rotate, they are going to be used directly in the shader
			// so there is never a conversion, and since the rotate function uses radians, we need to convert them back
			// negative, same thing that i need to think about to understand why
			//x
			tempConeDir[0] = coneDirection[0] + cos(-tempAxisRot[1]) / (PI / 180.0f);
			//y
			// adding this one was fairly simple, just wonder if we might have issues later on
			// since this is not a full spherical coordinate system
			tempConeDir[1] = coneDirection[1] + sin(tempAxisRot[2]) / (PI / 180.0f);
			//z
			tempConeDir[2] = coneDirection[2] + sin(-tempAxisRot[1]) / (PI / 180.0f);
			//not used
			tempConeDir[3] = coneDirection[3];

			renderer.SetSpotLights(&tempSpotLight, shaderArrayIndex);
			renderer.setSpotParam(&tempConeDir, cutOff, shaderArrayIndex);
		}
		else {
			renderer.SetSpotLights(&position, shaderArrayIndex);
			renderer.setSpotParam(&coneDirection, cutOff, shaderArrayIndex);
		}	
}