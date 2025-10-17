#include "AssimpImporter.h"
#include "Prefabs.h"

using namespace std;

Texture texture;

//###########################
//this can be removed, since we should be giving it the file path directly
//
/* Directory name containing the current OBJ file. The OBJ filename should be the same*/
char model_dir[50];
//###########################

//############
//these are going to disapear
void get_vertices_for_node(const aiNode* nd, const aiScene* scene, Mesh meshId)
{
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	//gets absolote max and min vertices of children
	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];

			//vec4 format for vector
			objectVertices[meshId].push_back(tmp.x);
			objectVertices[meshId].push_back(tmp.y);
			objectVertices[meshId].push_back(tmp.z);
			objectVertices[meshId].push_back(0.0f);
		}
	}

	// searches for max and min in submeshes of children
	for (n = 0; n < nd->mNumChildren; ++n) {
		get_vertices_for_node(nd->mChildren[n], scene, meshId);
	}
}


void get_vertices(const aiScene* scene, Mesh mesh)
{
	objectVertices[mesh] = {};
	get_vertices_for_node(scene->mRootNode, scene, mesh);
	objectNumberVertices[mesh] = objectVertices[mesh].size();
}
//################

/// Can't send color down as a pointer to aiColor4D because AI colors are ABGR.
//void Color4f(const aiColor4D *color)
//{
//	glColor4f(color->r, color->g, color->b, color->a);
//}

void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

void color4_to_float4(const aiColor4D* c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

//not too sure how it works, need to compare with the shader on the other side to understand
//i believe that as of now we arent really using this, although could be changed to create an automatic way of adding textures
bool LoadGLTexturesTUs(const aiScene*& scene, GLuint*& textureIds, unordered_map<std::string, GLuint>& textureIdMap)  // Create OGL textures objects and maps them to texture units.  
{
	aiString path;	// filename
	string filename;

	/* scan scene's materials for textures */
	for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
	{
		// o fragment shader suporta material com duas texturas difusas, 1 especular e 1 normal map
		for (unsigned int i = 0; i < scene->mMaterials[m]->GetTextureCount(aiTextureType_DIFFUSE); i++) {

			scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, i, &path);
			filename = model_dir;
			filename.append(path.data);
			//fill map with textures, OpenGL image ids set to 0
			textureIdMap[filename] = 0;
		}

		for (unsigned int i = 0; i < scene->mMaterials[m]->GetTextureCount(aiTextureType_SPECULAR); i++) {
			scene->mMaterials[m]->GetTexture(aiTextureType_SPECULAR, i, &path);
			filename = model_dir;
			filename.append(path.data);
			textureIdMap[filename] = 0;
		}

		for (unsigned int i = 0; i < scene->mMaterials[m]->GetTextureCount(aiTextureType_NORMALS); i++) {
			scene->mMaterials[m]->GetTexture(aiTextureType_NORMALS, i, &path);
			filename = model_dir;
			filename.append(path.data);
			textureIdMap[filename] = 0;
		}

	}

	int numTextures = textureIdMap.size();
	printf("numeros de mapas %d\n", numTextures);
	if (numTextures) {
		textureIds = new GLuint[numTextures];
		glGenTextures(numTextures, textureIds); /* Texture name generation */

		/* get iterator */
		unordered_map<std::string, GLuint>::iterator itr = textureIdMap.begin();
		filename = (*itr).first;  // get filename

		//##########
		//create the texture objects array and asssociate them with TU and place the TU in the key value of the map
		//CHANGE THIS, IT CANT DO AN ARBITRARY FOLDER
		for (int i = 0; itr != textureIdMap.end(); ++i, ++itr)
		{

			filename = (*itr).first;  // get filename
			std::cout << filename << std::endl;
			filename = "assets/backpack/" + filename;
			texture.texture2D_Loader(filename.c_str());
			//Texture2D_Loader(textureIds, filename.c_str(), i);  //it already performs glBindTexture(GL_TEXTURE_2D, textureIds[i])
			(*itr).second = i;	  // save texture unit for filename in map
			//printf("textura = %s  TU = %d\n", filename.c_str(), i);
		}
		//###########
	}
	return true;
}

// only needs bounding box init methods
bool Import3DFromFile(const std::string& pFile, Assimp::Importer& importer, const aiScene*& scene, Mesh mesh)
{
	//const aiScene* scene = NULL;

	scene = importer.ReadFile(pFile, aiProcessPreset_TargetRealtime_Quality | aiProcess_MakeLeftHanded | aiProcess_PreTransformVertices);

	// if the import fails, ReadFile() returns a NULL pointer.
	if (!scene)
	{
		printf("%s\n", importer.GetErrorString());
		return false;
	}

	// Now we can access the file's contents.
	printf("Import of scene %s succeeded.\n", pFile.c_str());

	//#########################################
	// what needs to be done is, the function should get all vertice positions and put them in the object vertices dic
	// instead of finding directly the max and min, the bounding box class does that
	get_vertices(scene, mesh);
	//################################

	// We're done. Everything will be cleaned up by the importer destructor
	return true;
}

vector<struct MyMesh> createMeshFromAssimp(const aiScene*& sc, GLuint*& textureIds) {

	//#########################
	//this is diferent from our usual mesh stack, should probably add it to the usual
	vector<struct MyMesh> myMeshes;
	//##########################

	GLuint buffer;

	// ########################
	// this might need to be taken away from here
	// 
	// unordered map which maps image filenames to texture units TU. This map is filled in the  LoadGLTexturesTUs()
	unordered_map<std::string, GLuint> textureIdMap;
	//###############################


	printf("Cena: numero total de malhas = %d\n", sc->mNumMeshes);


	//##
	// FOR NOW WE ARE NOT USING THIS, SO THERE IS NO POINT IN RUNNING IT
	//LoadGLTexturesTUs(sc, textureIds, textureIdMap); //it creates the unordered map which maps image filenames to texture units TU
	//##


	// For each mesh
	for (unsigned int n = 0; n < sc->mNumMeshes; ++n)
	{
		const aiMesh* mesh = sc->mMeshes[n];

		// create array with faces
		// have to convert from Assimp format to array
		unsigned int* faceArray;
		faceArray = (unsigned int*)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
		unsigned int faceIndex = 0;

		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const aiFace* face = &mesh->mFaces[t];

			//probs need to change this to 4, since we use vertices with 4 elements
			memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
			//then this goes to 4 too
			faceIndex += 3;
		}
		//after this we save the vertices for bounding box calc, since this is all from technicaly the same mesh
		// we can just add them all together in the save buffer

		struct MyMesh aMesh;

		//we also save this, add to the buffer everytime so we later iterate through them all
		aMesh.numIndexes = mesh->mNumFaces * 3;

		aMesh.type = GL_TRIANGLES;
		aMesh.mat.texCount = 0;

		// generate Vertex Array for mesh
		glGenVertexArrays(1, &(aMesh.vao));
		glBindVertexArray(aMesh.vao);

		// buffer for faces
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * aMesh.numIndexes, faceArray, GL_STATIC_DRAW);

		// buffer for vertex positions
		if (mesh->HasPositions()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(Shader::VERTEX_COORD_ATTRIB);
			glVertexAttribPointer(Shader::VERTEX_COORD_ATTRIB, 3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex normals
		if (mesh->HasNormals()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
			glEnableVertexAttribArray(Shader::NORMAL_ATTRIB);
			glVertexAttribPointer(Shader::NORMAL_ATTRIB, 3, GL_FLOAT, 0, 0, 0);
		}

		// buffers for vertex tangents and bitangents
		// CURRENTLY WE DO NOT HAVE TANGENTS OR BIT TANGENTS
		// it still runs even if we dont have a buffer place for it tho? i still reserves memory space so i commented it just in case
		/*if (mesh->HasTangentsAndBitangents()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mTangents, GL_STATIC_DRAW);
			glEnableVertexAttribArray(Shader::TANGENT_ATTRIB);
			glVertexAttribPointer(Shader::TANGENT_ATTRIB, 3, GL_FLOAT, 0, 0, 0);

			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mBitangents, GL_STATIC_DRAW);
			glEnableVertexAttribArray(Shader::BITANGENT_ATTRIB);
			glVertexAttribPointer(Shader::BITANGENT_ATTRIB, 3, GL_FLOAT, 0, 0, 0);
		}*/

		// buffer for vertex texture coordinates
		if (mesh->HasTextureCoords(0)) {
			float* texCoords = (float*)malloc(sizeof(float) * 2 * mesh->mNumVertices);
			for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {
				texCoords[k * 2] = mesh->mTextureCoords[0][k].x;
				texCoords[k * 2 + 1] = mesh->mTextureCoords[0][k].y;
			}
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh->mNumVertices, texCoords, GL_STATIC_DRAW);
			glEnableVertexAttribArray(Shader::TEXTURE_COORD_ATTRIB);
			glVertexAttribPointer(Shader::TEXTURE_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);
		}

		// unbind buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// create material; each mesh has ONE material
		aiMaterial* mtl = sc->mMaterials[mesh->mMaterialIndex];

		aiString texPath;	//contains filename of texture

		string filename;
		GLuint TU;
		unsigned int TUcount = 0;

		for (unsigned int i = 0; i < mtl->GetTextureCount(aiTextureType_DIFFUSE); i++) {

			mtl->GetTexture(aiTextureType_DIFFUSE, i, &texPath);
			filename = model_dir;
			filename.append(texPath.data);
			TU = textureIdMap[filename];

			aMesh.texUnits[TUcount] = TU;
			aMesh.texTypes[TUcount] = DIFFUSE;
			aMesh.mat.texCount = TUcount + 1;
			TUcount++;
		}

		for (unsigned int i = 0; i < mtl->GetTextureCount(aiTextureType_SPECULAR); i++) {
			mtl->GetTexture(aiTextureType_SPECULAR, i, &texPath);
			filename = model_dir;
			filename.append(texPath.data);
			TU = textureIdMap[filename];

			aMesh.texUnits[TUcount] = TU;
			aMesh.texTypes[TUcount] = SPECULAR;
			aMesh.mat.texCount = TUcount + 1;
			TUcount++;
		}
		for (unsigned int i = 0; i < mtl->GetTextureCount(aiTextureType_NORMALS); i++) {
			mtl->GetTexture(aiTextureType_NORMALS, i, &texPath);
			filename = model_dir;
			filename.append(texPath.data);
			TU = textureIdMap[filename];

			aMesh.texUnits[TUcount] = TU;
			aMesh.texTypes[TUcount] = NORMALS;
			aMesh.mat.texCount = TUcount + 1;
			TUcount++;
		}


		float c[4];
		set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
		aiColor4D diffuse;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
			color4_to_float4(&diffuse, c);
		memcpy(aMesh.mat.diffuse, c, sizeof(c));

		set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
		aiColor4D ambient;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
			color4_to_float4(&ambient, c);
		memcpy(aMesh.mat.ambient, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D specular;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
			color4_to_float4(&specular, c);
		memcpy(aMesh.mat.specular, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D emission;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
			color4_to_float4(&emission, c);
		memcpy(aMesh.mat.emissive, c, sizeof(c));

		float shininess = 0.0;
		unsigned int max;
		aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
		aMesh.mat.shininess = shininess;

		myMeshes.push_back(aMesh);
	}
	// cleaning up
	textureIdMap.clear();

	return(myMeshes);
}