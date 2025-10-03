#include "ObstacleNode.h"

bool ObstacleNode::ProcessNode() {

	// see if its within bounds
	if (!CheckDistanceFromMoveArea()) return false;

	// if within bounds move
	UpdateLocalTransform(Transform{
			new vec3{
				moveSpeed * (float)cos(- moveDirection),
				0.0f,
				moveSpeed * (float)sin(-moveDirection)
			},
			nullptr,
			new vec3{ 0.0f, 45.0f, 0.0f }
		}
	);

}

//returns true if within move area, false is otherwise
bool ObstacleNode::CheckDistanceFromMoveArea() {
	return sqrtf(
		(*localTransform.translation)[0] * (*localTransform.translation)[0] +
		(*localTransform.translation)[2] * (*localTransform.translation)[2]
	) <= moveRadius;
}