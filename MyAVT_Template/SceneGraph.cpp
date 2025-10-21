#include "SceneGraph.h"

void SceneGraph::InitializeSceneGraph() {
	mu.loadIdentity(gmu::VIEW);
	mu.loadIdentity(gmu::MODEL);

	renderer.SetPointLightsMode(pointLightMode);
	renderer.SetDirectionalLightMode(directionalLightMode);
	renderer.setSpotLightMode(spotLightMode);
	renderer.setFogMode(fogMode);
	renderer.setBumpMapMode(bumpMapMode);
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
AssimpNode* SceneGraph::AddAssimpNode(Mesh meshId, AssimpMeshData data, int textureId, Transform localTransform, Node* parent) {

	AssimpNode* newNode = new AssimpNode(meshId, data, textureId, localTransform, parent);

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
void SceneGraph::AddFloor(int meshId, int textureId, Transform localTransform, int pos) {
	floor[pos] = new Node(meshId, textureId, localTransform);
}

void SceneGraph::AddLight(LightNode* light) {
	lights.push_back(light);
}

void SceneGraph::DrawScene() {

	//stencil setup
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Cameras
	activeCamera->RenderCamera();

	//##########################
	// Planar Reflextions

	glStencilFunc(GL_NEVER, 0x2, 0xFF);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
	glStencilMask(0xFF);

	DrawNode(floor[0], false);

	glStencilFunc(GL_EQUAL, 0x2, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	//stop writing to stencil mask
	glStencilMask(0x00);

	for each(LightNode * light in lights) {
		light->position[1] = -light->position[1];
	}
	CalculateLights();

	glCullFace(GL_FRONT);
	for each(Node * child in head)
	{
		SceneGraph::InvDrawNode(child, false);
	}
	glCullFace(GL_BACK);

	//revert lights
	for each(LightNode * light in lights) {
		light->position[1] = -light->position[1];
	}
	CalculateLights();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		// Blend specular floor with reflected geometry

	DrawNode(floor[0], false);


	//###########################################################
	// SHADOW PASS
	float plane[4] = { 0.0f, 1.0f, 0.0f, 0.0f }; // plane equation y=0
	glDepthMask(GL_FALSE);
	//Dark the color stored in color buffer
	glBlendFunc(GL_DST_COLOR, GL_ZERO);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-0.5f, -0.5f);

	float mat[16];
	for each(LightNode* light in lights) {

		//skip non active light types
		if(light->GetType() == "PointLight" && !pointLightMode)
			continue;
		else if(light->GetType() == "SpotLight" && !spotLightMode)
			continue;
		else if(light->GetType() == "DirectionalLight" && !directionalLightMode)
			continue;

		//spotlight does not work, probably since it uses diferent logic from every other light
		//need to ask later about it
		mu.shadow_matrix(
			mat,
			plane,
			light->position  // light position
		);

	
		mu.pushMatrix(gmu::MODEL);
		mu.multMatrix(gmu::MODEL, mat);

		for each(Node * child in head)
		{
			SceneGraph::InvDrawNode(child, true);
		}
		mu.popMatrix(gmu::MODEL);
	}
	
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//##########################
	//Normal draw

	//part that is not drawn on stencil
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	//draw scene
	SceneGraph::DrawNode(floor[1], false);
	for each(Node * child in head)
	{
		SceneGraph::DrawNode(child, false);
	}
	//transparent part last
	//SceneGraph::DrawNode(floor[0]);

	//part drawn on stencil
	activeCamera->invCamera();
	//Lights
	CalculateLights();

	//stencil bit
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	
	//draw scene
	SceneGraph::DrawNode(floor[1], false);
	for each(Node * child in head)
	{
		SceneGraph::DrawNode(child, false);
	}
	//transparent part last
	//SceneGraph::DrawNode(floor[0]);
}

void SceneGraph::DrawNode(Node* node, bool shadowMode) {
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
		SceneGraph::DrawNode(child, shadowMode);
	}

	mu.computeDerivedMatrix(gmu::PROJ_VIEW_MODEL);
	mu.computeNormalMatrix3x3();

	data.meshID = node->meshId;

	if(!shadowMode)
		data.texMode = node->textureId;
	else
		data.texMode = 0;

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
void SceneGraph::InvDrawNode(Node* node, bool shadowMode) {
	dataMesh data;

	mu.pushMatrix(gmu::MODEL);

	if (node->localTransform.translation && node->GetParent() == nullptr)
		mu.translate(gmu::MODEL, (*node->localTransform.translation)[0], -(*node->localTransform.translation)[1], (*node->localTransform.translation)[2]);
	else if (node->localTransform.translation)
		mu.translate(gmu::MODEL, (*node->localTransform.translation)[0], (*node->localTransform.translation)[1], (*node->localTransform.translation)[2]);

	if (node->localTransform.rotation) {
		//rotate assumes that the angle is in degrees
		//this needs refactoring, it ruins quad normals
		mu.rotate(gmu::MODEL, (*node->localTransform.rotation)[1] / (PI / 180.0f), 0.0f, 1.0f, 0.0f);
		mu.rotate(gmu::MODEL, (*node->localTransform.rotation)[2] / (PI / 180.0f), 0.0f, 0.0f, 1.0f);
		mu.rotate(gmu::MODEL, (*node->localTransform.rotation)[0] / (PI / 180.0f), 1.0f, 0.0f, 0.0f);
	}

	if (node->localTransform.scale && node->GetParent() == nullptr)
		mu.scale(gmu::MODEL, (*node->localTransform.scale)[0], -(*node->localTransform.scale)[1], (*node->localTransform.scale)[2]);
	else if (node->localTransform.scale)
		mu.scale(gmu::MODEL, (*node->localTransform.scale)[0], (*node->localTransform.scale)[1], (*node->localTransform.scale)[2]);

	for each(Node * child in node->GetChildren())
	{
		SceneGraph::InvDrawNode(child, shadowMode);
	}

	mu.computeDerivedMatrix(gmu::PROJ_VIEW_MODEL);
	mu.computeNormalMatrix3x3();

	data.meshID = node->meshId;
	if (!shadowMode)
		data.texMode = node->textureId;
	else
		data.texMode = 0;
	data.vm = mu.get(gmu::VIEW_MODEL);
	data.pvm = mu.get(gmu::PROJ_VIEW_MODEL);
	data.normal = mu.getNormalMatrix();


	if (node->HasSubMeshes()) {

		std::array<int, 2>* bounds = node->GetBounds();
		if (!bounds) {
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