#pragma once
#include "Transform.h"
#include "Prefabs.h"

//forward declaration
class Node;

class BoundingBox {
public:
	BoundingBox(Node* parent) {

		this->parent = parent;

		initBounds();
	}

	void RecalculateBounds();

	bool CheckCollision(BoundingBox* other);

	const vec3 GetMaxBounds() { return minBounds; }
	const vec3 GetMinBounds() { return maxBounds; }

private:

	void initBounds();
	int calculateParentAccumulativeTransform(Node* parent);

	vec3 minBounds = { 0.0f, 0.0f, 0.0f };
	vec3 maxBounds = { 0.0f, 0.0f, 0.0f };

	Node* parent;

};