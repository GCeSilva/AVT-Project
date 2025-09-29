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

	float localPosition[3] = {0.0f, 0.0f, 0.0f};
	float lookTarget[3] = { 0.0f, 0.0f, 0.0f };
	float height = 0.0f, width = 0.0f;

	CameraState currentState = FollowPlayerPersp;

	Node* parent = nullptr;

	Camera(float pos[3], float target[3], Node* parent = nullptr) {
		memcpy(localPosition, pos, sizeof(float) * 3);
		memcpy(lookTarget, target, sizeof(float) * 3);

		this->parent = parent;
	}
	void RenderCamera();
private:
	void RenderFollow();
	void RenderTopDownPersp();
	void RenderTopDownOrtho();
};