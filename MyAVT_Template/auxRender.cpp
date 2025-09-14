#include "auxRender.h"

using namespace std;

std::unordered_map<MeshType, Transformations> meshTransformations = {
	{ FLOOR, {
		nullptr, 
		new Scale{ 1000.0f, 1000.0f, 1.0f }, 
		new Rotation{ -90.0f, 1.0f, 0.0f, 0.0f } 
	}},
	{ CUBE, {
		new Translation{ -0.5f, 0.0f, -0.5f },
		nullptr,
		nullptr
	}}
};

std::unordered_map<MeshType, Material> meshMaterials = {
	{ FLOOR, {
		{ 0.3f, 0.0f, 0.0f, 1.0f }, // Ambient
		{ 0.8f, 0.1f, 0.1f, 1.0f }, // Diffuse
		{ 0.3f, 0.3f, 0.3f, 1.0f }, // Specular
		{ 0.0f, 0.0f, 0.0f, 1.0f }, // Emissive
		100.0f, // Shininess
		0 // Texture Count
	}},
	{ CUBE, {
		{ 0.3f, 0.0f, 0.0f, 1.0f }, // Ambient
		{ 0.8f, 0.1f, 0.1f, 1.0f }, // Diffuse
		{ 0.3f, 0.3f, 0.3f, 1.0f }, // Specular
		{ 0.0f, 0.0f, 0.0f, 1.0f }, // Emissive
		100.0f, // Shininess
		0 // Texture Count
	}}
};

std::unordered_map<MeshType, std::function<MyMesh()>> meshCreators = {
	{ FLOOR, []() { return createQuad(1.0f, 1.0f); }},
	{ CUBE,  []() { return createCube(); }}
};

void createGeometry(MyMesh amesh, Material mat) {
	amesh.mat = mat;
	renderer.myMeshes.push_back(amesh);
}

void instantiate(int mesh, int tex, Transformations transform) {
	dataMesh data;

	mu.pushMatrix(gmu::MODEL);

	if (transform.rotation)
		mu.rotate(gmu::MODEL, (*transform.rotation)[0], (*transform.rotation)[1], (*transform.rotation)[2], (*transform.rotation)[3]);
	if (transform.scale)
		mu.scale(gmu::MODEL, (*transform.scale)[0], (*transform.scale)[1], (*transform.scale)[2]);
	if (transform.translation)
		mu.translate(gmu::MODEL, (*transform.translation)[0], (*transform.translation)[1], (*transform.translation)[2]);

	mu.computeDerivedMatrix(gmu::PROJ_VIEW_MODEL);
	mu.computeNormalMatrix3x3();

	data.meshID = mesh;
	data.texMode = tex;
	data.vm = mu.get(gmu::VIEW_MODEL);
	data.pvm = mu.get(gmu::PROJ_VIEW_MODEL);
	data.normal = mu.getNormalMatrix();

	renderer.renderMesh(data);
	mu.popMatrix(gmu::MODEL);
}