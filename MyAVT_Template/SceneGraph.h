#pragma once
#include <iostream>

#include "LightNode.h"
#include "ObstacleNode.h"
#include "Camera.h"
#include "Prefabs.h"
#include "AssimpNode.h"

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2

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
	bool fogMode;

	Camera* activeCamera = nullptr;

	void InitializeSceneGraph();

	std::list<Node*> GetGraph() { return head; }

	void AddLight(LightNode* light);

	void RemoveNode(Node* node);
	Node* AddNode(int meshId, int textureId, Transform localTransform, Node* parent = nullptr);
	AssimpNode* AddAssimpNode(Mesh meshId, AssimpMeshData data, int textureId, Transform localTransform, Node* parent = nullptr);

	//LATER JUST MAKE AN ABSTRACTION SO THAT ADD NODE CAN ADD BOTH NODES
	ObstacleNode* AddObstacle(int meshId, int textureId, Transform localTransform, std::array<float, 3> centre, Node* parent = nullptr);
	
	void DrawScene();

private:
	std::list<Node*> head;

	std::list<LightNode*> lights;

	void DrawNode(Node* node);
	void CalculateLights();
};

// mesh creation
void createGeometry(MyMesh amesh, Material mat);

//helper funcitons, later put them in their own .h file
void CreateCity(SceneGraph* sg, std::array<int, 2> domainX, std::array<int, 2> domainY, 
				int blockSize, float distanceBlocks, float percentDistBuildings);
float lerp(float from, float to, float step);

