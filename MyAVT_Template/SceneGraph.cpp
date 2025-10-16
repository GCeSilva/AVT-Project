#include "SceneGraph.h"

void SceneGraph::InitializeSceneGraph() {
	mu.loadIdentity(gmu::VIEW);
	mu.loadIdentity(gmu::MODEL);

	renderer.SetPointLightsMode(pointLightMode);
	renderer.SetDirectionalLightMode(directionalLightMode);
	renderer.setSpotLightMode(spotLightMode);
	renderer.setFogMode(fogMode);
}
void SceneGraph::CalculateLights() {

	for each(LightNode * light in lights)
	{
		light->CalculateLight(&light->shaderArrayIndex);
	}
}

void SceneGraph::RemoveNode(Node* node) {
	if (node->GetParent()) {
		node->GetParent()->RemoveNode(node);
	}
	else {
		head.remove(node);
	}
	delete node;
}

Node* SceneGraph::AddNode(int meshId, int textureId, Transform localTransform, Node* parent) {
	
	Node* newNode = new Node(meshId, textureId, localTransform, parent);

	if (parent) 
		parent->AddChild(newNode);
	else 
		head.push_back(newNode);

	return newNode;
}
AssimpNode* SceneGraph::AddAssimpNode(int startMeshId, int endMeshId, int textureId, Transform localTransform, Node* parent) {

	AssimpNode* newNode = new AssimpNode(startMeshId, endMeshId, textureId, localTransform, parent);

	if (parent)
		parent->AddChild(newNode);
	else
		head.push_back(newNode);

	return newNode;
}
ObstacleNode* SceneGraph::AddObstacle(int meshId, int textureId, Transform localTransform, std::array<float, 3> centre, Node* parent) {
	ObstacleNode* newNode = new ObstacleNode(meshId, textureId, localTransform, centre, parent);
	if (parent)
		parent->AddChild(newNode);
	else
		head.push_back(newNode);
	return newNode;
}

void SceneGraph::AddLight(LightNode* light) {
	lights.push_back(light);
}

void SceneGraph::DrawScene() {

	//Cameras
	activeCamera->RenderCamera();

	//Lights
	CalculateLights();

	//Objects
	for each(Node * child in head)
	{
		SceneGraph::DrawNode(child);
	}
}

void SceneGraph::DrawNode(Node* node) {
	dataMesh data;

	mu.pushMatrix(gmu::MODEL);
	
	if (node->localTransform.translation)
		mu.translate(gmu::MODEL, (*node->localTransform.translation)[0], (*node->localTransform.translation)[1], (*node->localTransform.translation)[2]);

	if (node->localTransform.rotation) {
		//rotate assumes that the angle is in degrees
		//this needs refactoring, it ruins quad normals
		mu.rotate(gmu::MODEL, (*node->localTransform.rotation)[1] / (PI / 180.0f), 0.0f, 1.0f, 0.0f);
		mu.rotate(gmu::MODEL, (*node->localTransform.rotation)[2] / (PI / 180.0f), 0.0f, 0.0f, 1.0f);
		mu.rotate(gmu::MODEL, (*node->localTransform.rotation)[0] / (PI / 180.0f), 1.0f, 0.0f, 0.0f);
	}

	if (node->localTransform.scale)
		mu.scale(gmu::MODEL, (*node->localTransform.scale)[0], (*node->localTransform.scale)[1], (*node->localTransform.scale)[2]);

	for each(Node* child in node->GetChildren())
	{
		SceneGraph::DrawNode(child);
	}

	mu.computeDerivedMatrix(gmu::PROJ_VIEW_MODEL);
	mu.computeNormalMatrix3x3();

	data.meshID = node->meshId;
	data.texMode = node->textureId;
	data.vm = mu.get(gmu::VIEW_MODEL);
	data.pvm = mu.get(gmu::PROJ_VIEW_MODEL);
	data.normal = mu.getNormalMatrix();


	if (node->HasSubMeshes()) {

		std::array<int, 2>* bounds = node->GetBounds();
		if(!bounds) {
			std::cout << "null pointer on bound check of submesh drawing" << std::endl;
			exit(0);
		}

		// this feels kinda scuffed, but it seems to work
		for (int i = (*bounds)[0]; i <= (*bounds)[1]; i++) {
			data.meshID = i;
			renderer.renderMesh(data);
		}
	}
	else
		renderer.renderMesh(data);
	
	mu.popMatrix(gmu::MODEL);
}

// mesh creation
void createGeometry(MyMesh amesh, Material mat) {
	amesh.mat = mat;
	renderer.myMeshes.push_back(amesh);
}

// helper functions

void CreateCity(SceneGraph* sg ,std::array<int, 2> domainX, std::array<int, 2> domainY, 
				int blockSize, float distanceBlocks, float percentDistBuildings) 
{
	Node* tmpNode;

	//sorry not sorry
	for (int x = domainX[0]; x <= domainX[1]; x++) {
		if (x == 0) continue; //leave the center free

		for (int y = domainY[0]; y <= domainY[1]; y++) {
			if (y == 0) continue; //leave the center free

			for (int i = 0; i < blockSize; i++) {
				for (int j = 0; j < blockSize; j++) {

					tmpNode = sg->AddNode(CUBE, 2, objectTransforms[BUILDING]);

					tmpNode->UpdateLocalTransform(Transform{
						new vec3{
							//this one between blocks
							((float)x * (distanceBlocks + (*objectTransforms[BUILDING].scale)[0] * blockSize + (blockSize - 1) * (*objectTransforms[BUILDING].scale)[0] * percentDistBuildings)) +
							//this one between buildings
							((float)i * ((*objectTransforms[BUILDING].scale)[0] + (*objectTransforms[BUILDING].scale)[0] * percentDistBuildings)) * (x / -x),

							0.0f,

							((float)y * (distanceBlocks + (*objectTransforms[BUILDING].scale)[2] * blockSize + (blockSize - 1) * (*objectTransforms[BUILDING].scale)[0] * percentDistBuildings)) +
							((float)j * ((*objectTransforms[BUILDING].scale)[2] + (*objectTransforms[BUILDING].scale)[2] * percentDistBuildings)) * (x / -x)
							},
							nullptr,
							nullptr
						}
					);

				}
			}

		}
	}
}

float lerp(float from, float to, float step) {
	return (1 - step) * from + step * to;
}