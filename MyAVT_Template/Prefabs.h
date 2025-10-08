#pragma once
#include <unordered_map>
#include <functional>

#include "Model.h"
#include "Transform.h"
#include "mathUtility.h"

enum MaterialConfigs {
	DEFAULT,
	TRANSLUCENT
};

// list of pre set objects
enum Objects {
	FLOOR,
	BUILDING,
	DRONEBODY,
	BIGBALL
};

// later change these to their own .h file
extern std::unordered_map<Mesh, std::function<MyMesh()>> meshCreators;

extern std::unordered_map<MaterialConfigs, Material> meshMaterials;

// pre set objects
extern std::unordered_map<Objects, Transform> objectTransforms;

// for bounding boxes
extern std::unordered_map<Mesh, std::vector<float>> objectVertices;
extern std::unordered_map<Mesh, int> objectNumberVertices;