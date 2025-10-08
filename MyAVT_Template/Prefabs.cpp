#include "Prefabs.h"

// later change these to their own .h file
std::unordered_map<Mesh, std::function<MyMesh()>> meshCreators = {
	{ QUAD,		[]() { return createQuad(1.0f, 1.0f);			}},
	{ CUBE,		[]() { return createCube();						}},
	{ SPHERE,	[]() { return createSphere(1.0f, 16);			}},
	{ TORUS,	[]() { return createTorus(0.5f, 1.0f, 32, 16);	}},
	{ CYLINDER, []() { return createCylinder(1.0f, 0.5f, 32);	}},
	{ CONE,		[]() { return createCone(1.0f, 0.5f, 32);		}},
	{ PAWN,		[]() { return createPawn();						}},
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
	{ TRANSLUCENT, {
		{ 0.3f, 0.0f, 0.0f, 0.5f }, // Ambient
		{ 0.8f, 0.1f, 0.1f, 0.5f }, // Diffuse
		{ 0.3f, 0.3f, 0.3f, 0.5f }, // Specular
		{ 0.0f, 0.0f, 0.0f, 0.5f }, // Emissive
		100.0f, // Shininess
		0 // Texture Count
	}}
};

// pre set objects
std::unordered_map<Objects, Transform> objectTransforms = {
	{ FLOOR, {
		nullptr,
		new vec3 { 1000.0f, 1000.0f, 0.0f },
		new vec3 { -90.0f, 1.0f, 0.0f }
	}},
	{ BUILDING, {
		new vec3 { 0.0f, 0.5f, 0.0f },
		new vec3 { 2.0f, 5.0f, 2.0f },
		nullptr
	}},
	{ DRONEBODY, {
		new vec3 { 0.0f, 3.0f, 0.0f },
		new vec3 { 1.0f, 0.4f, 0.6f },
		new vec3 { 0.0f, 0.0f, 0.0f }
	}},
	{ BIGBALL, {
		new vec3 { 0.0f, 15.0f, 0.0f },
		new	vec3 { 5.0f, 5.0f, 5.0f },
		nullptr
	}}
};

std::unordered_map<Mesh, float*> objectVertices;
std::unordered_map<Mesh, int> objectNumberVertices;