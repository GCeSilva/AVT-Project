#pragma once
#include "mathUtility.h"
#include "Node.h"

extern gmu mu;

class Camera {
public:
	enum CameraState {
		FollowPlayerPersp,
		TopDownPersp,
		TopDownOrtho
	};

	float lookTarget[3] = { 0.0f, 0.0f, 0.0f };
	float height = 0.0f, width = 0.0f;

	float radious = 45.0f;
	float localRotation[3] = { 18.0f, 0.0f, -90.0f };

	CameraState currentState = FollowPlayerPersp;

	Node* parent = nullptr;

	Camera(float target[3], Node* parent = nullptr) {
		memcpy(lookTarget, target, sizeof(float) * 3);

		this->parent = parent;
	}
	void RenderCamera();
private:
	void RenderFollow();
	void RenderTopDownPersp();
	void RenderTopDownOrtho();
};