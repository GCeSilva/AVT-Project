#pragma once
#include "Node.h"


//ADD PROPER HIERITANCE OF TRANSFORMATIONS(????)
class ObstacleNode : public Node
{
public:
	ObstacleNode(int meshId, int textureID, Transform localTransform, std::array<float, 3> centre, Node* parent = nullptr)
		: Node(meshId, textureID, localTransform, parent) 
	{
		this->centre = centre;
		//random angle between 0 and 359
		moveDirection = 360 * (((float)(rand()) / (float)(RAND_MAX)));
		moveSpeed = moveSpeed * ((float)(rand()) / (float)(RAND_MAX)+0.1f);

		float x = ((float)(rand()) / (float)(RAND_MAX)) * pow(-1, rand() % 100 + 1);
		float y = ((float)(rand()) / (float)(RAND_MAX)) * pow(-1, rand() % 100 + 1);

		(*localTransform.rotation)[1] = moveDirection;

		this->localTransform.translation = new vec3{
			(x * moveRadius * (float)cos(moveDirection * (PI / 180.0f))) + this->centre[0],
			3.0f,
			(y * moveRadius * (float)sin(moveDirection * (PI / 180.0f))) + this->centre[2]
		};;
	}

	bool ProcessNode();

	bool CollisionBehaviour(Node* other) override;

private:
	// this is a rotation angle, which tells forward direction, also in radians
	float moveDirection = 0.0f;
	// this will be our center of the area of movement
	std::array<float, 3> centre = { 0.0f, 0.0f, 0.0f };
	float moveRadius = 50.0f;
	float moveSpeed = 0.7f;
	
	bool CheckDistanceFromMoveArea();
};