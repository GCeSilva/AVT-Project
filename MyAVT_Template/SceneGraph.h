#pragma once
#include <list>
#include <array>
#include <unordered_map>
#include <functional>

#include "mathUtility.h"
#include "renderer.h"
#include "model.h"


extern gmu mu;
extern Renderer renderer;

using Translation = std::array<float, 3>;
using Scale = std::array<float, 3>;
using Rotation = std::array<float, 4>;

struct Transform {
	Translation* translation;
	Scale* scale;
	Rotation* rotation;

	Transform operator+(const Transform &other) const {
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

class SceneGraph {
public:

	struct Node {
	public:
		int meshId;
		int textureId;
		Transform localTransform;

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
			localTransform += additiveTransform;
		}

	private:
		Node* parent;
		std::list<Node*> children = {};
	};

	void InitializeSceneGraph();
	Node* AddNode(int meshId, int textureId, Transform localTransform, Node* parent = nullptr);
	void DrawScene();

private:
	std::list<Node*> head;

	void DrawNode(Node* node);
};

// mesh creation
enum Mesh {
	QUAD,
	CUBE,
	SPHERE,
	TORUS,
	CYLINDER,
	CONE,
	PAWN
};
enum MaterialConfigs {
	DEFAULT,
};

void createGeometry(MyMesh amesh, Material mat);
extern std::unordered_map<MaterialConfigs, Material> meshMaterials;
extern std::unordered_map<Mesh, std::function<MyMesh()>> meshCreators;

// list of pre set objects
enum Objects {
	FLOOR,
	BUILDING,
	DRONEBODY
};
extern std::unordered_map<Objects, Transform> objectTransforms;

