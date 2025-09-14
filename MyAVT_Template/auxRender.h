#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <array>
#include <string>

#include "mathUtility.h"
#include "renderer.h"
#include "model.h"

using Translation = std::array<float, 3>;
using Scale = std::array<float, 3>;
using Rotation = std::array<float, 4>;

extern gmu mu;
extern Renderer renderer;

enum MeshType {
	FLOOR,
	CUBE,
	SPHERE,
	CYLINDER,
	CONE
};

enum TransformationType {
	TRANSLATION,
	SCALE,
	ROTATION
};

struct Transformations {
	Translation *translation;
	Scale *scale;
	Rotation *rotation;
};

extern std::unordered_map<MeshType, Transformations> meshTransformations;
extern std::unordered_map<MeshType, Material> meshMaterials;
extern std::unordered_map<MeshType, std::function<MyMesh()>> meshCreators;

void createGeometry(MyMesh amesh, Material mat);
void instantiate(int mesh, int tex, Transformations transformations);