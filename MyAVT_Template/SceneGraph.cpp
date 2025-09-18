#include "SceneGraph.h"

void SceneGraph::InitializeSceneGraph() {
	mu.loadIdentity(gmu::VIEW);
	mu.loadIdentity(gmu::MODEL);
}

SceneGraph::Node* SceneGraph::AddNode(int meshId, int textureId, Transform localTransform, Node* parent) {
	
	Node* newNode = new Node(meshId, textureId, localTransform, parent);

	if (parent) 
		parent->AddChild(newNode);
	else 
		head.push_back(newNode);

	return newNode;
}

void SceneGraph::DrawScene() {
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
		nullptr,
		nullptr
	}}
};