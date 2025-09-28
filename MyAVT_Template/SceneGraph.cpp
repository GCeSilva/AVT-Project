#include "SceneGraph.h"

void SceneGraph::InitializeSceneGraph() {
	mu.loadIdentity(gmu::VIEW);
	mu.loadIdentity(gmu::MODEL);

	renderer.SetPointLightsMode(pointLightMode);
	renderer.SetDirectionalLightMode(directionalLightMode);
	renderer.setSpotLightMode(spotLightMode);
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
		mu.rotate(gmu::MODEL, node->axisRotations[1] / (PI / 180.0f), 0.0f, 1.0f, 0.0f);
		mu.rotate(gmu::MODEL, node->axisRotations[2] / (PI / 180.0f), 0.0f, 0.0f, 1.0f);
		mu.rotate(gmu::MODEL, node->axisRotations[0] / (PI / 180.0f), 1.0f, 0.0f, 0.0f);
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

	renderer.renderMesh(data);
	mu.popMatrix(gmu::MODEL);
}

// mesh creation
void createGeometry(MyMesh amesh, Material mat) {
	amesh.mat = mat;
	renderer.myMeshes.push_back(amesh);
}

	
// later change these to their own .h file
std::unordered_map<Mesh, std::function<MyMesh()>> meshCreators = {
	{ QUAD, []() { return createQuad(1.0f, 1.0f); }},
	{ CUBE,  []() { return createCube(); }},
	{ SPHERE,  []() { return createSphere(1.0f, 16); }},
	{ TORUS,  []() { return createTorus(0.5f, 1.0f, 32, 16); }},
	{ CYLINDER,  []() { return createCylinder(1.0f, 0.5f, 32); }},
	{ CONE,  []() { return createCone(1.0f, 0.5f, 32); }},
	{ PAWN,  []() { return createPawn(); }},
};

std::unordered_map<MaterialConfigs, Material> meshMaterials = {
	{ DEFAULT, {
		{ 0.3f, 0.0f, 0.0f, 1.0f }, // Ambient
		{ 0.8f, 0.1f, 0.1f, 1.0f }, // Diffuse
		{ 0.3f, 0.3f, 0.3f, 1.0f }, // Specular
		{ 0.0f, 0.0f, 0.0f, 1.0f }, // Emissive
		100.0f, // Shininess
		0 // Texture Count
	}},
};

// pre set objects
std::unordered_map<Objects, Transform> objectTransforms = {
	{ FLOOR, {
		nullptr,
		new Scale{ 1000.0f, 1000.0f, 0.0f },
		new Rotation{ -90.0f, 1.0f, 0.0f, 0.0f }
	}},
	{ BUILDING, {
		new Translation{0.0f, 0.5f, 0.0f},
		new Scale{ 2.0f, 5.0f, 2.0f},
		nullptr
	}},
	{ DRONEBODY, {
		new Translation{ 0.0f, 3.0f, 0.0f},
		new Scale{ 1.0f, 0.4f, 0.6f },
		new Rotation {0.0f, 0.0f, 0.0f, 0.0f}
	}},
};

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
						new Translation{
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