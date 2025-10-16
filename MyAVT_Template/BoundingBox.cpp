#include "BoundingBox.h"
//completes forward declaration and avoids circular dependency
#include "Node.h"
#include "mathUtility.h"
#include <iostream>

gmu mu = getMu();

// must be done after initializing so that the stack is in identity
void BoundingBox::RecalculateBounds() {
	// remember the vertex are a VEC4!!

	int parentCount = calculateParentAccumulativeTransform(parent);

	float vertex[4] = {
			objectVertices[(Mesh)parent->meshId][0],
			objectVertices[(Mesh)parent->meshId][1],
			objectVertices[(Mesh)parent->meshId][2],
			1.0f
	};
	float resVertex[4];
	mu.multMatrixPoint(gmu::MODEL, vertex, resVertex);

	//need to reset them so we can get new values and not compete with the old ones
	maxBounds = { resVertex[0], resVertex[1], resVertex[2] };
	minBounds = { resVertex[0], resVertex[1], resVertex[2] };

	//apply to vertex and find mins and maxs
	for (int i = 0; i < objectNumberVertices[(Mesh)parent->meshId]; i += 4) {

		float vertex[4] = { 
			objectVertices[(Mesh)parent->meshId][i], 
			objectVertices[(Mesh)parent->meshId][i + 1], 
			objectVertices[(Mesh)parent->meshId][i + 2], 
			1.0f 
		};
		float resVertex[4];
		mu.multMatrixPoint(gmu::MODEL, vertex, resVertex);

		for (int j = 0; j < 3; ++j) {
			maxBounds[j] = std::max(maxBounds[j], resVertex[j]);
			minBounds[j] = std::min(minBounds[j], resVertex[j]);
		}
	
	}
	//clear stack
	for(int i = 0; i < parentCount; i++)
		mu.popMatrix(gmu::MODEL);
}
// ahah sadge
int BoundingBox::calculateParentAccumulativeTransform(Node* parent) {

	int i = 1;

	if (parent->GetParent())
		i += calculateParentAccumulativeTransform(parent->GetParent());

	mu.pushMatrix(gmu::MODEL);

	if (parent->localTransform.translation)
		mu.translate(gmu::MODEL, (*parent->localTransform.translation)[0], (*parent->localTransform.translation)[1], (*parent->localTransform.translation)[2]);

	if (parent->localTransform.rotation) {
		//rotate assumes that the angle is in degrees
		mu.rotate(gmu::MODEL, (*parent->localTransform.rotation)[1] / (PI / 180.0f), 0.0f, 1.0f, 0.0f);
		mu.rotate(gmu::MODEL, (*parent->localTransform.rotation)[2] / (PI / 180.0f), 0.0f, 0.0f, 1.0f);
		mu.rotate(gmu::MODEL, (*parent->localTransform.rotation)[0] / (PI / 180.0f), 1.0f, 0.0f, 0.0f);
	}

	if (parent->localTransform.scale)
		mu.scale(gmu::MODEL, (*parent->localTransform.scale)[0], (*parent->localTransform.scale)[1], (*parent->localTransform.scale)[2]);

	return i;
}

// ;-;
void BoundingBox::initBounds() {
	mu.loadIdentity(gmu::MODEL);
	RecalculateBounds();
}

bool BoundingBox::CheckCollision(BoundingBox* other) {


	//checking if we are inside
	bool xW = ((other->minBounds[0] <= minBounds[0] && minBounds[0] <= other->maxBounds[0]) || (other->minBounds[0] <= maxBounds[0] && maxBounds[0] <= other->maxBounds[0]));
	bool yW = ((other->minBounds[1] <= minBounds[1] && minBounds[1] <= other->maxBounds[1]) || (other->minBounds[1] <= maxBounds[1] && maxBounds[1] <= other->maxBounds[1]));
	bool zW = ((other->minBounds[2] <= minBounds[2] && minBounds[2] <= other->maxBounds[2]) || (other->minBounds[2] <= maxBounds[2] && maxBounds[2] <= other->maxBounds[2]));

	//checking if they are inside
	bool xT = ((minBounds[0] <= other->minBounds[0] && other->minBounds[0] <= maxBounds[0]) || (minBounds[0] <= other->maxBounds[0] && other->maxBounds[0] <= maxBounds[0]));
	bool yT = ((minBounds[1] <= other->minBounds[1] && other->minBounds[1] <= maxBounds[1]) || (minBounds[1] <= other->maxBounds[1] && other->maxBounds[1] <= maxBounds[1]));
	bool zT = ((minBounds[2] <= other->minBounds[2] && other->minBounds[2] <= maxBounds[2]) || (minBounds[2] <= other->maxBounds[2] && other->maxBounds[2] <= maxBounds[2]));

	return	(xT || xW) && (yT || yW) && (zT || zW);
}