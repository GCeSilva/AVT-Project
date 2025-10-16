#pragma once
#include <list>
#include <array>
#include <iostream>
#include "BoundingBox.h"

class Node {
public:
	int meshId;
	int textureId;
	Transform localTransform;
	BoundingBox* boundingBox = nullptr;


	Node(int meshId, int textureID, Transform localTransform, Node* parent = nullptr)
	{

		this->meshId = meshId;
		this->textureId = textureID;
		this->parent = parent;

		this->localTransform = localTransform;
		if (localTransform.rotation) {
			//saving them as radians
			(*localTransform.rotation)[0] = (*localTransform.rotation)[0] * (PI / 180.0f);

			(*localTransform.rotation)[1] = (*localTransform.rotation)[1] * (PI / 180.0f);

			(*localTransform.rotation)[2] = (*localTransform.rotation)[2] * (PI / 180.0f);
		}

		this->boundingBox = new BoundingBox{ this };
	}

	virtual bool CollisionBehaviour(Node* other) { return true; }

	void AddChild(Node* child) { this->children.push_back(child); }
	void RemoveNode(Node* node) { this->children.remove(node); }
	Node* GetParent() { return this->parent; }
	std::list<Node*> GetChildren() { return this->children; }
	
	//these are only for inheritance of AssimpNode
	bool HasSubMeshes() { return this->hasSubMeshes; }
	virtual std::array<int, 2>* GetBounds() { return nullptr; };

	//since we will be adding upon our local transforms
	//this might hell have to write a little less
	//could this allow negative axis values?
	void UpdateLocalTransform(Transform additiveTransform) {
		localTransform += additiveTransform;
		boundingBox->RecalculateBounds();
	}

protected:
	bool hasSubMeshes = false;

private:
	Node* parent;
	std::list<Node*> children = {};

};
