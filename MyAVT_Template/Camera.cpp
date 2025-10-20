#include "Camera.h"
#include "SceneGraph.h"
#include <GL/glew.h>


void Camera::RenderCamera() {

	switch (currentState)
	{
	case Camera::FollowPlayerPersp:
		RenderFollow();
		break;
	case Camera::TopDownPersp:
		RenderTopDownPersp();
		break;
	case Camera::TopDownOrtho:
		RenderTopDownOrtho();
		break;
	default:
		break;
	}
}

void Camera::RenderTopDownPersp() {
	// Prevent a divide by zero, when window is too short
	if (height == 0)
		height = 1;

	// set the viewport to be the entire window
	glViewport(0, 0, width, height);

	// set the projection matrix
	float ratio = (1.0f * width) / height;
	mu.loadIdentity(gmu::PROJECTION);
	mu.perspective(53.13f, ratio, 0.1f, 1000.0f);

	float tempPos[3] = { 0.0f, 0.0f, 0.0f };
	Node* tempParent = parent;
	while (tempParent != nullptr) {
		tempPos[0] = (*parent->localTransform.translation)[0];
		tempPos[1] = (*parent->localTransform.translation)[1];
		tempPos[2] = (*parent->localTransform.translation)[2];

		tempParent = tempParent->GetParent();
	}

	mu.lookAt(
		2.0f, 100.0f, 0.0f,
		lookTarget[0], lookTarget[1], lookTarget[2],
		0, 1, 0
	);
}

void Camera::RenderTopDownOrtho() {
	// Prevent a divide by zero, when window is too short
	if (height == 0)
		height = 1;

	// set the viewport to be the entire window
	glViewport(0, 0, width, height);

	// set the projection matrix
	mu.loadIdentity(gmu::PROJECTION);
	mu.ortho(-width/12, width/12, -height/12, height/12, 0.1f, 1000.0f);

	float tempPos[3] = { 0.0f, 0.0f, 0.0f };
	Node* tempParent = parent;
	while (tempParent != nullptr) {
		tempPos[0] = (*parent->localTransform.translation)[0];
		tempPos[1] = (*parent->localTransform.translation)[1];
		tempPos[2] = (*parent->localTransform.translation)[2];

		tempParent = tempParent->GetParent();
	}

	mu.lookAt(
		2.0f, 100.0f, 0.0f,
		lookTarget[0], lookTarget[1], lookTarget[2],
		0, 1, 0
	);
}

void Camera::RenderFollow() {
	// Prevent a divide by zero, when window is too short
	if (height == 0)
		height = 1;

	// set the viewport to be the entire window
	glViewport(0, 0, width, height);

	RenderRearView();

	// set the projection matrix
	float ratio = (1.0f * width) / height;
	mu.loadIdentity(gmu::PROJECTION);
	mu.perspective(53.13f, ratio, 0.1f, 1000.0f);

	float parentPos[3] = { 0.0f, 0.0f, 0.0f };
	float parentRot[3] = { 0.0f, 0.0f, 0.0f };
	Node* tempParent = parent;
	while (tempParent != nullptr) {
		parentPos[0] = (*parent->localTransform.translation)[0];
		parentPos[1] = (*parent->localTransform.translation)[1];
		parentPos[2] = (*parent->localTransform.translation)[2];

		parentRot[0] = (*parent->localTransform.rotation)[0];
		parentRot[1] = (*parent->localTransform.rotation)[1];
		parentRot[2] = (*parent->localTransform.rotation)[2];

		tempParent = tempParent->GetParent();
	}

	float tempRot[3];
	//local Rotated position + parent rot
	tempRot[0] = radious * sin(localRotation[2] * 3.14f / 180.0f + parentRot[1]) * cos(localRotation[0] * 3.14f / 180.0f);
	tempRot[1] = radious * sin(localRotation[0] * 3.14f / 180.0f);
	tempRot[2] = radious * cos(localRotation[2] * 3.14f / 180.0f + parentRot[1]) * cos(localRotation[0] * 3.14f / 180.0f);

	mu.lookAt(
		//  rotPos          adding pos to rot around
		tempRot[0] + parentPos[0],
		tempRot[1] + parentPos[1],
		tempRot[2] + parentPos[2],
		parentPos[0],
		parentPos[1],
		parentPos[2],
		0, 1, 0
	);
}

void Camera::RenderRearView() {
	
	//setting up camera

	// set the projection matrix
	mu.loadIdentity(gmu::PROJECTION);
	mu.ortho(-1, 1, -1, 1, 0.1f, 1000.0f);

	mu.lookAt(
		0.0f, 3.0f, 1.0f,
		0.0f, 3.0f, 0.0f,
		0, 1, 0
	);

	//setting up stencil
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	SceneGraph::DrawNode(stencilShape);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	glEnable(GL_DEPTH_TEST);


	mu.loadIdentity(gmu::VIEW);

}

void Camera::invCamera() {

	mu.loadIdentity(gmu::VIEW);

	float parentPos[3] = { 0.0f, 0.0f, 0.0f };
	float parentRot[3] = { 0.0f, 0.0f, 0.0f };
	Node* tempParent = parent;
	while (tempParent != nullptr) {
		parentPos[0] = (*parent->localTransform.translation)[0];
		parentPos[1] = (*parent->localTransform.translation)[1];
		parentPos[2] = (*parent->localTransform.translation)[2];

		parentRot[0] = (*parent->localTransform.rotation)[0];
		parentRot[1] = (*parent->localTransform.rotation)[1];
		parentRot[2] = (*parent->localTransform.rotation)[2];

		tempParent = tempParent->GetParent();
	}

	float tempRot[3];
	//local Rotated position + parent rot
	tempRot[0] = radious * sin(localRotation[2] * 3.14f / 180.0f + parentRot[1]) * cos(localRotation[0] * 3.14f / 180.0f);
	tempRot[1] = radious * sin(localRotation[0] * 3.14f / 180.0f);
	tempRot[2] = radious * cos(localRotation[2] * 3.14f / 180.0f + parentRot[1]) * cos(localRotation[0] * 3.14f / 180.0f);

	mu.lookAt(
		//  rotPos          adding pos to rot around
		parentPos[0],
		parentPos[1] - 1.0f,
		parentPos[2],

		tempRot[0] + parentPos[0],
		-tempRot[1] + parentPos[1] - 1.0f,
		tempRot[2] + parentPos[2],

		0, 1, 0
	);
}