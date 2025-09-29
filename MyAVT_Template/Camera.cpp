#include "Camera.h"

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

void Camera::RenderFollow() {
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
		localPosition[0] + tempPos[0], localPosition[1] + tempPos[1], localPosition[2] + tempPos[2],
		(*parent->localTransform.translation)[0], (*parent->localTransform.translation)[1], (*parent->localTransform.translation)[2],
		0, 1, 0
	);
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
	float ratio = (1.0f * width) / height;
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