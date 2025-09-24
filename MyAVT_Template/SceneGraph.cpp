#include "SceneGraph.h"

void SceneGraph::InitializeSceneGraph() {
	mu.loadIdentity(gmu::VIEW);
	mu.loadIdentity(gmu::MODEL);

	renderer.SetPointLightsMode(pointLightMode);
	renderer.SetDirectionalLightMode(directionalLightMode);
	renderer.setSpotLightMode(spotLightMode);
}
void SceneGraph::CalculateLights() {

	//Point Lights
	float tempPointLight[NUM_POINT_LIGHTS][4] = {};
	int i = 0;
	for each(PointLight* light in pointLights)
	{
		if (light->parent) {
			tempPointLight[i][0] = (*light->parent->localTransform.translation)[0] + light->position[0];
			tempPointLight[i][1] = (*light->parent->localTransform.translation)[0] + light->position[1];
			tempPointLight[i][2] = (*light->parent->localTransform.translation)[0] + light->position[2];
			tempPointLight[i][3] = (*light->parent->localTransform.translation)[0] + light->position[3];
		}
		else {
			tempPointLight[i][0] = light->position[0];
			tempPointLight[i][1] = light->position[1];
			tempPointLight[i][2] = light->position[2];
			tempPointLight[i][3] = light->position[3];
		}
		i++;
	}
	renderer.SetPointLights(&tempPointLight);

	//Directional Light
	renderer.SetDirectionalLight(directionalLights);

	//Spot Lights
	float tempSpotLight[NUM_SPOT_LIGHTS][4] = {};
	float tempConeDir[NUM_SPOT_LIGHTS][4] = {};
	float tempCutOff[NUM_SPOT_LIGHTS];
	i = 0;
	for each(SpotLight* light in spotLights) {

		if (light->parent) {

			tempSpotLight[i][0] = light->position[0] + (*light->parent->localTransform.translation)[0];
			tempSpotLight[i][1] = light->position[1] + (*light->parent->localTransform.translation)[1];
			tempSpotLight[i][2] = light->position[2] + (*light->parent->localTransform.translation)[2];
			tempSpotLight[i][3] = light->position[3];

			// later turn this into proper spherical coordinates
			tempConeDir[i][0] = light->coneDirection[0] + cos(light->parent->axisRotations[1]) / (3.14159265358979323846f / 180.0f);
			tempConeDir[i][1] = light->coneDirection[1];
			tempConeDir[i][2] = light->coneDirection[2] + sin(light->parent->axisRotations[1]) / (3.14159265358979323846f / 180.0f);
			tempConeDir[i][3] = light->coneDirection[3];

		}
		else {
			tempSpotLight[i][0] = light->position[0];
			tempSpotLight[i][1] = light->position[1];
			tempSpotLight[i][2] = light->position[2];
			tempSpotLight[i][3] = light->position[3];

			tempConeDir[i][0] = light->coneDirection[0];
			tempConeDir[i][1] = light->coneDirection[1];
			tempConeDir[i][2] = light->coneDirection[2];
			tempConeDir[i][3] = light->coneDirection[3];
		}

		tempCutOff[i] = light->cutOff;
	}

	renderer.SetSpotLights(&tempSpotLight);
	renderer.setSpotParam(&tempConeDir, tempCutOff);

}

SceneGraph::Node* SceneGraph::AddNode(int meshId, int textureId, Transform localTransform, Node* parent) {
	
	Node* newNode = new Node(meshId, textureId, localTransform, parent);

	if (parent) 
		parent->AddChild(newNode);
	else 
		head.push_back(newNode);

	return newNode;
}

void SceneGraph::AddPointLight(float pos[4], Node* parent) {
	pointLights.push_back(new PointLight{ {pos[0], pos[1], pos[2], pos[3]}, parent });
}
void SceneGraph::AddDirectionalLight(float pos[4]) {
	directionalLights[0] = pos[0];
	directionalLights[1] = pos[1];
	directionalLights[2] = pos[2];
	directionalLights[3] = pos[3];
}
void SceneGraph::AddSpotLight(float pos[4], float coneDir[4], float cutOffAngle, Node* parent) {
	spotLights.push_back(new SpotLight{ 
			{pos[0], pos[1], pos[2], pos[3]}, 
			{coneDir[0], coneDir[1], coneDir[2], coneDir[3]}, 
			cutOffAngle, 
			parent 
		}
	);
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
	if (node->localTransform.rotation)
		mu.rotate(gmu::MODEL, (*node->localTransform.rotation)[0], (*node->localTransform.rotation)[1], (*node->localTransform.rotation)[2], (*node->localTransform.rotation)[3]);
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
		nullptr
	}},
};

// helper functions

void CreateCity(SceneGraph* sg ,std::array<int, 2> domainX, std::array<int, 2> domainY, 
				int blockSize, float distanceBlocks, float percentDistBuildings) 
{
	SceneGraph::Node* tmpNode;

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