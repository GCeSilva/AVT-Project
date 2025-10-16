#pragma once
#include <assert.h>
#include <stdlib.h>
#include <unordered_map>
#include <iostream>

#include "mathUtility.h"
#include "model.h"
#include "texture.h"
#include "shader.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"


bool Import3DFromFile(const std::string& pFile, Assimp::Importer& importer, const aiScene*& sc);
std::vector<struct MyMesh> createMeshFromAssimp(const aiScene*& sc, GLuint*& textureIds);