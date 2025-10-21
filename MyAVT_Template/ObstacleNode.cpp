#include "ObstacleNode.h"
#include "Prefabs.h"

bool ObstacleNode::ProcessNode() {

	// see if its within bounds
	if (!CheckDistanceFromMoveArea()) return false;

	// if within bounds move
	UpdateLocalTransform(Transform{
			new vec3{
				moveSpeed * (float)cos(-moveDirection),
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

bool ObstacleNode::CollisionBehaviour(Node* other){

	//not needed anymore

	//other->localTransform.translation = new vec3{ 0.0f, 3.0f, 0.0f };

	//other->localTransform.rotation = new vec3{0.0f, 0.0f, 0.0f};

	//so it doesnt roll back the movement before the teleport
	return false;
}