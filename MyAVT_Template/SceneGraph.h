#pragma once
#include <unordered_map>
#include <functional>

#include "mathUtility.h"
#include "model.h"
#include "LightNode.h"

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2

extern gmu mu;

class SceneGraph {
public:

	struct SpotLight {
		float position[4];
		float coneDirection[4];
		float cutOff;
		Node* parent;
	};
	struct PointLight
	{
		float position[4];
		Node* parent;
	};

	bool spotLightMode;
	bool pointLightMode;
	bool directionalLightMode;

	void InitializeSceneGraph();

	void AddLight(LightNode* light);

	Node* AddNode(int meshId, int textureId, Transform localTransform, Node* parent = nullptr);
	void DrawScene();

private:
	std::list<Node*> head;

	std::list<LightNode*> lights;

	void DrawNode(Node* node);
	void CalculateLights();
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

//helper funcitons, later put them in their own .h file
void CreateCity(SceneGraph* sg, std::array<int, 2> domainX, std::array<int, 2> domainY, 
				int blockSize, float distanceBlocks, float percentDistBuildings);
float lerp(float from, float to, float step);

