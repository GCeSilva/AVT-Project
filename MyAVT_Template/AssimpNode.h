#pragma once
#include "Node.h"
#include "Prefabs.h"

/*
* 
* needs bounding box implementation still
* only the backpack textures are getting messed up, much like the teacher demo
* 
* could also do that when we get the mesh vector, we save the position of the initial and final mesh
*	and instead of doing it like we are, we could simply make a function that makes a parent node,
*	and then makes all submeshes son nodes using the indicies to know when to start and stop 
*	and then run this as any other normal node
* 
*/

struct AssimpMeshData {
	int startMeshId, endMeshId;
};

class AssimpNode : public Node {
public:
	AssimpNode(int meshId, AssimpMeshData data, int textureId, Transform localTransform, Node* parent = nullptr) :
		Node(meshId, textureId, localTransform, parent) {

		this->startMeshId = data.startMeshId;
		this->endMeshId = data.endMeshId;

		this->hasSubMeshes = true;

		float tmp;
		tmp = boundingBox->GetMaxBounds()[0] - boundingBox->GetMinBounds()[0];
		tmp = boundingBox->GetMaxBounds()[1] - boundingBox->GetMinBounds()[1] > tmp ? 
			boundingBox->GetMaxBounds()[1] - boundingBox->GetMinBounds()[1] : tmp;
		tmp = boundingBox->GetMaxBounds()[2] - boundingBox->GetMinBounds()[2] > tmp ? 
			boundingBox->GetMaxBounds()[2] - boundingBox->GetMinBounds()[2] : tmp;
		float scaleFactor = 1.f / tmp;

		this->localTransform.scale = new vec3{
			scaleFactor,
			scaleFactor,
			scaleFactor
		};

		this->boundingBox->RecalculateBounds();
	}

	std::array<int, 2>* GetBounds() override { return new std::array<int, 2>{ startMeshId, endMeshId }; }

private:
	int startMeshId, endMeshId;
};