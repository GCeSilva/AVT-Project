#pragma once
#include "Transform.h"

class BoundingBox {
public:
	BoundingBox(float* vertex, int vertexCount, Transform parentTransform) {
		//copy vertices
		memcpy(vertices, vertex, sizeof(float) * vertexCount);
		this->vertexCount = vertexCount;

		//apply parent transform to make sure vertices are correct

	}

	void RecalculateBounds();

	bool CheckCollision();

private:
	vec3 minBounds = { 0.0f, 0.0f, 0.0f };
	vec3 maxBounds = { 0.0f, 0.0f, 0.0f };

	float* vertices;
	int vertexCount;

};