#pragma once
#include <list>
#include <array>

#define PI 3.14159265358979323846f

using Translation = std::array<float, 3>;
using Scale = std::array<float, 3>;
using Rotation = std::array<float, 4>;

struct Transform {
	Translation* translation;
	Scale* scale;
	Rotation* rotation;

	Transform operator+(const Transform& other) const {
		return Transform{
			new Translation{
				(translation ? (*translation)[0] : 0) + (other.translation ? (*other.translation)[0] : 0),
				(translation ? (*translation)[1] : 0) + (other.translation ? (*other.translation)[1] : 0),
				(translation ? (*translation)[2] : 0) + (other.translation ? (*other.translation)[2] : 0)
			},
			(!scale && !other.scale) ? nullptr : new Scale{
				(scale ? (*scale)[0] : 1) + (other.scale ? (*other.scale)[0] : 0),
				(scale ? (*scale)[1] : 1) + (other.scale ? (*other.scale)[1] : 0),
				(scale ? (*scale)[2] : 1) + (other.scale ? (*other.scale)[2] : 0),
			},
			(!rotation && !other.rotation) ? nullptr : new Rotation{
				(rotation ? (*rotation)[0] : 0) + (other.rotation ? (*other.rotation)[0] : 0),
				(rotation ? (*rotation)[1] : 0) + (other.rotation ? (*other.rotation)[1] : 0),
				(rotation ? (*rotation)[2] : 0) + (other.rotation ? (*other.rotation)[2] : 0),
				(rotation ? (*rotation)[3] : 0) + (other.rotation ? (*other.rotation)[3] : 0),
			}
		};
	}
	Transform operator+=(const Transform& other) {
		*this = *this + other;
		return *this;
	}
};

class Node {
public:
	int meshId;
	int textureId;
	Transform localTransform;

	std::array<float, 3> axisRotations = { 0, 0, 0 };

	Node(int meshId, int textureID, Transform localTransform, Node* parent = nullptr)
	{
		this->meshId = meshId;
		this->textureId = textureID;
		this->localTransform = localTransform;
		this->parent = parent;
	}
	void AddChild(Node* child) { this->children.push_back(child); }
	std::list<Node*> GetChildren() { return this->children; }

	//since we will be adding upon our local transforms
	//this might hell have to write a little less
	void UpdateLocalTransform(Transform additiveTransform) {
		if (additiveTransform.rotation) {
			axisRotations[0] -= (*additiveTransform.rotation)[1] >= 0 ?
				(*additiveTransform.rotation)[0] * (PI / 180.0f) : 0.0f;
			axisRotations[1] -= (*additiveTransform.rotation)[2] >= 0 ?
				(*additiveTransform.rotation)[0] * (PI / 180.0f) : 0.0f;
			axisRotations[2] -= (*additiveTransform.rotation)[3] >= 0 ?
				(*additiveTransform.rotation)[0] * (PI / 180.0f) : 0.0f;
		}
		localTransform += additiveTransform;
	}

private:
	Node* parent;
	std::list<Node*> children = {};
};
