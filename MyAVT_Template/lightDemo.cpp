//
// AVT 2025: Texturing with Phong Shading and Text rendered with TrueType library
// The text rendering was based on https://dev.to/shreyaspranav/how-to-render-truetype-fonts-in-opengl-using-stbtruetypeh-1p5k
// You can also learn an alternative with FreeType text: https://learnopengl.com/In-Practice/Text-Rendering
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
// 
// Author: João Madeiras Pereira
//

#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>

// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <IL/il.h>

#include "shader.h"
#include "texture.h"
#include "SceneGraph.h"
#include "AssimpImporter.h"

#define THRESHOLD 0.01f

using namespace std;

#define CAPTION "AVT 2025 Welcome Demo"
int WindowHandle = 0;
int WinX = 1024, WinY = 768;

unsigned int FrameCount = 0;

//File with the font
const string fontPathFile = "fonts/arial.ttf";


//Object of class renderer to manage the rendering of meshes and ttf-based bitmap text
Renderer renderer;

SceneGraph sg;
Node* drone;
//temporary
Node* tree;

// Controls
std::array<int, 8> speedKeys;
// SPEED IS MEANT TO INCREMENT AND DECREMENT AS WE CLICK THE KEYS
float maxSpeed = 0.5f;
float acceleration = 0.1f;
float rotationSpeed = 5.0f;

// obstacles
#define maxObstacles 10
ObstacleNode* obstacles[maxObstacles];
	
// Camera Position
Camera* camera;

// Camera Spherical Coordinates
float alpha = 57.0f, beta = 18.0f;
float r = 5.0f;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];

//lights
// probably at some point remove this from here and put it on its own file or something
float directionalLightPos[4] = { 1000.0f, 1000.0f, 1.0f, 0.0f };
bool directionalLightMode = true;

float pointLightPos[NUM_POINT_LIGHTS][4] = {
	{  5.0f,  10.0f,   5.0f, 1.0f },
	{ -5.0f,  10.0f,   5.0f, 1.0f },
	{  5.0f,  10.0f,  -5.0f, 1.0f },
	{ -5.0f,  10.0f,  -5.0f, 1.0f },
	{  0.0f,  10.0f,   0.0f, 1.0f },
	{  5.0f,  10.0f,   0.0f, 1.0f }
};
bool pointLightMode = true;

//Spotlight
float spotLightPos[NUM_SPOT_LIGHTS][4] = {
	{  1.0f, 0.0f, 0.0f, 1.0f },
	{ -1.0f, 0.0f, 0.0f, 1.0f }
};
float coneDir[NUM_SPOT_LIGHTS][4] = { 
	{ 0.0f, -0.0f, -1.0f, 0.0f },
	{ 0.0f, -0.0f, -1.0f, 0.0f }
};
float cutOff[NUM_SPOT_LIGHTS] = { 0.93f, 0.93f };
bool spotLightMode = true;

bool fogMode = true;
bool bumpMapMode = true;

bool fontLoaded = false;

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {
	camera->height = h;
	camera->width = w;
}


// ------------------------------------------------------------
//
// Render stufff
//

// forward anim vars
float animSpeed = 0.1f;
float tiltAngle = -(15.0f * (PI / 180.0f));
// side to side anim vars
float sideTiltAngle = -(10.0f * (PI / 180.0f));

bool animations() {
	//put real time transforms here
	for each(Node * child in drone->GetChildren())
	{
		child->UpdateLocalTransform(Transform{
			nullptr,
			nullptr,
			new vec3{ 0.0f, 5.0f, 0.0f }
			});
	}
	//tree sprite billboarding
	float tempRot[3];
	tempRot[0] = camera->radious * sin(camera->localRotation[2] * 3.14f / 180.0f + (*camera->GetParent()->localTransform.rotation)[1]) * cos(camera->localRotation[0] * 3.14f / 180.0f);
	tempRot[1] = camera->radious * sin(camera->localRotation[0] * 3.14f / 180.0f);
	tempRot[2] = camera->radious * cos(camera->localRotation[2] * 3.14f / 180.0f + (*camera->GetParent()->localTransform.rotation)[1]) * cos(camera->localRotation[0] * 3.14f / 180.0f);

	float alpha = atan2f(
		(tempRot[0] + (*camera->GetParent()->localTransform.translation)[0]) - (*tree->localTransform.translation)[0],
		(tempRot[2] + (*camera->GetParent()->localTransform.translation)[2]) - (*tree->localTransform.translation)[2]
	);

	tree->localTransform.rotation = new vec3{
		0.0f,
		alpha,
		0.0f
	};

	//#######################
	float transformCheck = 0.0f;

	if (speedKeys[0] != 0 || speedKeys[1] != 0) {
		// never forgeti, this is in radians
		transformCheck = lerp((*drone->localTransform.rotation)[2], (speedKeys[1] + speedKeys[0]) * tiltAngle, animSpeed);
		transformCheck = abs(transformCheck) <= THRESHOLD ? 0.0f : transformCheck;
		(*drone->localTransform.rotation)[2] = transformCheck;
	}
	else {
		transformCheck = lerp((*drone->localTransform.rotation)[2], 0, animSpeed);
		transformCheck = abs(transformCheck) <= THRESHOLD ? 0.0f : transformCheck;
		(*drone->localTransform.rotation)[2] = transformCheck;
	}

	if (speedKeys[2] != 0 || speedKeys[3] != 0) {
		transformCheck = lerp((*drone->localTransform.rotation)[0], (speedKeys[3] + speedKeys[2]) * sideTiltAngle, animSpeed);
		transformCheck = abs(transformCheck) <= THRESHOLD ? 0.0f : transformCheck;
		(*drone->localTransform.rotation)[0] = transformCheck;
	}
	else {
		transformCheck = lerp((*drone->localTransform.rotation)[0], 0, animSpeed);
		transformCheck = abs(transformCheck) <= THRESHOLD ? 0.0f : transformCheck;
		(*drone->localTransform.rotation)[0] = transformCheck;
	}

	if (speedKeys[6] != 0 || speedKeys[7] != 0) {
		transformCheck = lerp((*drone->localTransform.rotation)[0], -(speedKeys[6] + speedKeys[7]) * tiltAngle, animSpeed);
		transformCheck = abs(transformCheck) <= THRESHOLD ? 0.0f : transformCheck;
		(*drone->localTransform.rotation)[0] = transformCheck;
	}
	else {
		transformCheck = lerp((*drone->localTransform.rotation)[0], 0, animSpeed);
		transformCheck = abs(transformCheck) <= THRESHOLD ? 0.0f : transformCheck;
		(*drone->localTransform.rotation)[0] = transformCheck;
	}

	if (transformCheck != 0)
		return true;

	return false;
}

bool droneCollisionCheck() {
	for each(Node* node in sg.GetGraph())
	{
		if (node == drone) continue;
		if (drone->boundingBox->CheckCollision(node->boundingBox))
			return node->CollisionBehaviour(drone);
	}
	for each(Node* node in sg.GetFloor())
	{
		if (drone->boundingBox->CheckCollision(node->boundingBox))
			return node->CollisionBehaviour(drone);
	}
	return false;
}

float currentUp = 0.0f;
float currentSide = 0.0f;
float currentSpeed = 0.0f;

void applyKeys(bool ani) {
	// key input output
	// WE NEED TO ADD TILT TO THE MOVEMENT AND ALSO MOVE THE LIGTHS ACCORDINGLY
	float tempAngle = 0.0f;
	if (speedKeys[2] != 0 || speedKeys[3]) {
		tempAngle = rotationSpeed * (speedKeys[2] + speedKeys[3]);
	}

	float targetSpeed = speedKeys[0] != 0 || speedKeys[1] != 0 ? maxSpeed * (speedKeys[0] + speedKeys[1]) : 0.0f;

	float upSpeed = speedKeys[4] != 0 || speedKeys[5] != 0 ? maxSpeed * (speedKeys[4] + speedKeys[5]) : 0.0f;

	float sideSpeed = speedKeys[6] != 0 || speedKeys[7] != 0 ? maxSpeed * (speedKeys[6] + speedKeys[7]) : 0.0f;


	//interpolation for smoother movement
	currentSpeed = lerp(currentSpeed, targetSpeed, acceleration);
	currentUp = lerp(currentUp, upSpeed, acceleration);
	currentSide = lerp(currentSide, sideSpeed, acceleration);

	// theres still these shenanigans :<
	currentSpeed = abs(currentSpeed) <= THRESHOLD ? 0.0f : currentSpeed;
	currentUp = abs(currentUp) <= THRESHOLD ? 0.0f : currentUp;
	currentSide = abs(currentSide) <= THRESHOLD ? 0.0f : currentSide;

	//bounds updated every frame now i guess
	drone->UpdateLocalTransform(Transform{
			new vec3{
			// this is using radians, since later the rotate is also in radians
			// need to think why it needs to be negative
				currentSpeed * cos(-(*drone->localTransform.rotation)[1]) + currentSide * sin((*drone->localTransform.rotation)[1]),
				currentUp,
				currentSpeed * sin(-(*drone->localTransform.rotation)[1]) + currentSide * cos((*drone->localTransform.rotation)[1])
			},
			nullptr,
			new vec3{ 0.0f, tempAngle, 0.0f }
		}
	);
	
	if (droneCollisionCheck()) {
		drone->UpdateLocalTransform(Transform{
			new vec3{
				// this is using radians, since later the rotate is also in radians
				// need to think why it needs to be negative
					-(currentSpeed * cos(-(*drone->localTransform.rotation)[1]) + currentSide * sin((*drone->localTransform.rotation)[1])),
					-currentUp,
					-(currentSpeed * sin(-(*drone->localTransform.rotation)[1]) + currentSide * cos((*drone->localTransform.rotation)[1]))
				},
				nullptr,
				new vec3{ 0.0f, -tempAngle, 0.0f }
			}
		);
		currentSpeed = 0.0f;
		currentSide = 0.0f;
		currentUp = 0.0f;
	}
		
}

void obstacleBehaviour() {

	for (int i = 0; i < maxObstacles; i++) {

		if (!obstacles[i]) continue;

		bool safe = obstacles[i]->ProcessNode();

		if (!safe) {
			sg.RemoveNode(obstacles[i]);
			obstacles[i] = sg.AddObstacle(CUBE, 3, objectTransforms[DRONEBODY], std::array<float, 3> {0.0f, 0.0f, 0.0f});
		}
	}
}

void renderSim(void) {

	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer.activateRenderMeshesShaderProg(); // use the required GLSL program to draw the meshes with illumination
	//Associar os Texture Units aos Objects Texture
	//stone.tga loaded in TU0; checker.tga loaded in TU1;  lightwood.tga loaded in TU2 ; mosaic.tga loaded in TU3
	renderer.setTexUnit(0, 0);
	renderer.setTexUnit(1, 1);
	renderer.setTexUnit(2, 2);
	renderer.setTexUnit(3, 3);

	//assimp test
	renderer.setTexUnit(4, 4);
	renderer.setTexUnit(5, 5);
	renderer.setTexUnit(6, 6);

	renderer.setTexUnit(7, 7);
	renderer.setTexUnit(8, 8);

	// load identity matrices
	sg.InitializeSceneGraph();

	obstacleBehaviour();

	bool ani = animations();
	
	applyKeys(ani);

	sg.DrawScene();

	//Render text (bitmap fonts) in screen coordinates. So use ortoghonal projection with viewport coordinates.
	//Each glyph quad texture needs just one byte color channel: 0 in background and 1 for the actual character pixels. Use it for alpha blending
	//text to be rendered in last place to be in front of everything
	/*
	if(fontLoaded) {
		glDisable(GL_DEPTH_TEST);
		TextCommand textCmd = { "AVT 2025 Welcome:\nGood Luck!", {100, 200}, 0.5 };
		//the glyph contains transparent background colors and non-transparent for the actual character pixels. So we use the blending
		glEnable(GL_BLEND);  
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		int m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		//viewer at origin looking down at  negative z direction

		mu.loadIdentity(gmu::MODEL);
		mu.loadIdentity(gmu::VIEW);
		mu.pushMatrix(gmu::PROJECTION);
		mu.loadIdentity(gmu::PROJECTION);
		mu.ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
		mu.computeDerivedMatrix(gmu::PROJ_VIEW_MODEL);
		textCmd.pvm = mu.get(gmu::PROJ_VIEW_MODEL);
		renderer.renderText(textCmd);
		mu.popMatrix(gmu::PROJECTION);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		
	}
	*/
	glutSwapBuffers();
}
// ------------------------------------------------------------
//
// Events from the Keyboard
//

void SpecialInput(int key, int x, int y) {

	//pitch
	if (key == GLUT_KEY_UP)
		speedKeys[0] = 1;

	if (key == GLUT_KEY_DOWN)
		speedKeys[1] = -1;

	if (key == GLUT_KEY_RIGHT)
		speedKeys[6] = 1;

	if (key == GLUT_KEY_LEFT)
		speedKeys[7] = -1;
		
}

void processKeys(unsigned char key, int xx, int yy)
{
	if (key == 27) glutLeaveMainLoop();

	if (key == 'c') sg.pointLightMode = !sg.pointLightMode;

	if (key == 'n') sg.directionalLightMode = !sg.directionalLightMode;

	if (key == 'h') sg.spotLightMode = !sg.spotLightMode;

	if (key == 'f') sg.fogMode = !sg.fogMode;

	if (key == 'v') sg.bumpMapMode = !sg.bumpMapMode;


	// up and own keys
	if (key == 'w') speedKeys[4] = 1;

	if (key == 's') speedKeys[5] = -1;

	//yaw
	if (key == 'a') speedKeys[2] = 1;

	if (key == 'd') speedKeys[3] = -1;


	if (key == '1') camera->currentState = Camera::FollowPlayerPersp;

	if (key == '2') camera->currentState = Camera::TopDownPersp;

	if (key == '3') camera->currentState = Camera::TopDownOrtho;

	if (key == 'm') glEnable(GL_MULTISAMPLE);

	if (key == 'b') glDisable(GL_MULTISAMPLE);

	if (key == 'r') { //reset
		alpha = 57.0f; beta = 18.0f;  // Camera Spherical Coordinates
		r = 5.0f;
		camera->radious = r = 5.0f;
		camera->localRotation[2] = alpha = -90.0f;
		camera->localRotation[0] = beta = 18.0f;
	}
}
void SpecialInputUp(int key, int x, int y) {
	//pitch
	if (key == GLUT_KEY_UP)
		speedKeys[0] = 0;

	if (key == GLUT_KEY_DOWN)
		speedKeys[1] = 0;

	if (key == GLUT_KEY_RIGHT)
		speedKeys[6] = 0;

	if (key == GLUT_KEY_LEFT)
		speedKeys[7] = 0;
}

void processKeysUp(unsigned char key, int xx, int yy)
{
	if (key == 'w') speedKeys[4] = 0;

	if (key == 's') speedKeys[5] = 0;

	if (key == 'a') speedKeys[2] = 0;

	if (key == 'd') speedKeys[3] = 0;
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// no touch-i when not correct state
	if (camera->currentState != Camera::FollowPlayerPersp) return;

	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			r += (yy - startY) * 0.01f;
			if (r < 0.1f)
				r = 0.1f;
		}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{
	// no touch-i when not correct state
	if (camera->currentState != Camera::FollowPlayerPersp) return;

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX =  - xx + startX;
	deltaY =    yy - startY;

	// left mouse button: move camera
	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;
		rAux = r;
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);
		if (rAux < 0.1f)
			rAux = 0.1f;
	}

	camera->localRotation[0] = betaAux;
	camera->localRotation[2] = alphaAux;
	camera->radious = rAux;

//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {
	// no touch-i when not correct state
	if (camera->currentState != Camera::FollowPlayerPersp) return;

	r -= direction * 1.0f;
	if (r < 0.1f)
		r = 0.1f;

	camera->radious = r;

//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}


//
// Scene building with basic geometry
//

Node* AssimpImport(std::string filepath, Mesh mesh) {
	const aiScene* scene;
	Assimp::Importer import;
	GLuint* textId;

	//import
	if (!Import3DFromFile(filepath, import, scene, mesh)) {
		std::cout << "FAILED IMPORT OF 3D FROM FILE" << std::endl;
		exit(0);
	}

	//turn in vector of MyMesh
	std::vector<MyMesh> obj = createMeshFromAssimp(scene, textId);
	
	std::cout << renderer.myMeshes.size() << std::endl;

	assimpMeshMap[mesh] = AssimpMeshData();
	assimpMeshMap[mesh].startMeshId = renderer.myMeshes.size();

	//add to mesh vector
	for each(MyMesh mesh in obj)
	{
		renderer.myMeshes.push_back(mesh);
	}

	assimpMeshMap[mesh].endMeshId = renderer.myMeshes.size() - 1;
	std::cout << renderer.myMeshes.size() << std::endl;
}

void buildScene()
{
	//Texture Object definition
	renderer.TexObjArray.texture2D_Loader("assets/stone.tga");
	renderer.TexObjArray.texture2D_Loader("assets/checker.png");
	renderer.TexObjArray.texture2D_Loader("assets/lightwood.tga");
	renderer.TexObjArray.texture2D_Loader("assets/mosaic.tga");


	//assimp
	// JUST MISSING THIS BEING SET AUTOMATICALLY ON FUNCTION CALL
	// we could mby load all required textures and assign an assimp object the needed texture so when it renders it then checks and uses them
	// instead of it being loaded in the renderSim
	renderer.TexObjArray.texture2D_Loader("assets/backpack/diffuse.jpg");
	renderer.TexObjArray.texture2D_Loader("assets/backpack/specular.jpg");
	renderer.TexObjArray.texture2D_Loader("assets/backpack/normal.png");

	renderer.TexObjArray.texture2D_Loader("assets/tree.png");

	renderer.TexObjArray.texture2D_Loader("assets/normal.tga");


	//Scene geometry with triangle meshes

	createGeometry(
		meshCreators[QUAD](),
		meshMaterials[DEFAULT]
	);
	createGeometry(
		meshCreators[CUBE](),
		meshMaterials[DEFAULT]
	);
	createGeometry(
		meshCreators[SPHERE](),
		meshMaterials[TRANSLUCENT]
	);
	createGeometry(
		meshCreators[TORUS](),
		meshMaterials[DEFAULT]
	);
	createGeometry(
		meshCreators[CYLINDER](),
		meshMaterials[DEFAULT]
	);
	createGeometry(
		meshCreators[CONE](),
		meshMaterials[TRANSLUCENT]
	);
	createGeometry(
		meshCreators[PAWN](),
		meshMaterials[TRANSLUCENT]
	);
	createGeometry(
		meshCreators[QUAD](),
		meshMaterials[TRANSLUCENT]
	);


	//Assimp asset import
	//AssimpImport("assets/backpack/backpack.obj", BACKPACK);
	AssimpImport("assets/drone/Drone.fbx", COTTAGE);

	/*sg.AddAssimpNode(BACKPACK, assimpMeshMap[BACKPACK], 4, Transform{
		new vec3{ 5.0f, 1.0f, 0.0f },
		new vec3{ 1.0f, 1.0f, 1.0f },
		new vec3{ 0.0f, 0.0f, 180.0f }
	});
	*/
	/*sg.AddAssimpNode(COTTAGE, assimpMeshMap[COTTAGE], 1, Transform{
		new vec3{ 0.0f, 1.0f ,0.0f},
		new vec3{ 1.0f, 1.0f ,1.0f},
		new vec3{ 0.0f, 0.0f ,180.0f},
	});*/	

	//floor
	sg.AddFloor(QUADTRANS, 0, objectTransforms[FLOOR], 0);

	sg.AddFloor(1, 2, Transform{
		new vec3{0.0f, -1.0f, 0.0f},
		new vec3{1000.0f, 0.01f, 1000.0f},
		new vec3{0.0f, 0.0f, 0.0f}
	}, 1);


	//twee
	tree = sg.AddNode(QUAD, 5, Transform{
		new vec3{0.0f, 1.5f, 0.0f},
		new vec3{2.0f, 2.0f, 2.0f},
		new vec3{0.0f, 0.0f, 0.0f}
	});

	// buildings
	std::array<int, 2> domainX = { -2, 2 };
	std::array<int, 2> domainY = { -2, 2 };
	float pdb = 0.5f; //50% of the building size
	float dbb = 5.0f;
	int blockSize = 5;

	CreateCity(&sg, domainX, domainY, blockSize, dbb, pdb);

	//drone
	drone = sg.AddNode(CUBE, 10, objectTransforms[DRONEBODY]);
	
	sg.AddNode(CUBE, 10, Transform{
		new vec3{ 1.0f, 1.0f,  1.0f},
		new vec3{ 0.2f, 0.2f,  0.2f},
		nullptr
	}, drone);
	sg.AddNode(CUBE, 10, Transform{
		new vec3{ 1.0f, 1.0f, -1.0f},
		new vec3{ 0.2f, 0.2f,  0.2f},
		nullptr
	}, drone);
	sg.AddNode(CUBE, 10, Transform{
		new vec3{-1.0f, 1.0f,  1.0f},
		new vec3{ 0.2f, 0.2f,  0.2f},
		nullptr
	}, drone);
	sg.AddNode(CUBE, 10, Transform{
		new vec3{-1.0f, 1.0f, -1.0f},
		new vec3{ 0.2f, 0.2f,  0.2f},
		nullptr
	}, drone);
	
	//obstacle
	for (int i = 0; i < maxObstacles; i++) {
		obstacles[i] = sg.AddObstacle(CUBE, 3, objectTransforms[DRONEBODY], std::array<float, 3> {0.0f, 0.0f, 0.0f});
	}
	
	//BigBall
	sg.AddNode(SPHERE, 2, objectTransforms[BIGBALL]);
	
	//lights
	sg.directionalLightMode = directionalLightMode;
	sg.pointLightMode = pointLightMode;
	sg.spotLightMode = spotLightMode;
	// fog
	sg.fogMode = fogMode;
	sg.bumpMapMode = bumpMapMode;

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		sg.AddLight(new PointLightNode(pointLightPos[i]));
	}

	sg.AddLight(new DirectionalLightNode(directionalLightPos));
	
	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		sg.AddLight(new SpotLightNode(spotLightPos[i], coneDir[i], cutOff[i], drone));
	}

	//camera
	float target[3] = { 0.0f, 0.0f, 0.0f };
	camera = new Camera{ target, new Node(QUAD, 0, Transform{
		new vec3{0.0f,3.8f,0.0f},
		new vec3{1.0f,0.4f,1.0f},
		new vec3{0.0f,0.0f,180.0f}
	}), drone};
	//little hack
	alpha = camera->localRotation[2];
	beta = camera->localRotation[0];
	r = camera->radious;

	sg.activeCamera = camera;

	//The truetypeInit creates a texture object in TexObjArray for storing the fontAtlasTexture
	
	fontLoaded = renderer.truetypeInit(fontPathFile);
	if (!fontLoaded)
		cerr << "Fonts not loaded\n";
	else 
		cerr << "Fonts loaded\n";

	printf("\nNumber of Texture Objects is %d\n\n", renderer.TexObjArray.getNumTextureObjects());
}

/// ::::::::::::::::::::::::::::::::::::::::::::::::CALLBACK FUNCIONS:::::::::::::::::::::::::::::::::::::::::::::::::://///

void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
	FrameCount = 0;
	glutTimerFunc(1000, timer, 0); //ms, chama a funcao timer, valor irrelevante
}

void refresh(int value)
{
	renderSim();
	glutTimerFunc(1000 / 60, refresh, 0);
}

// ------------------------------------------------------------
//
// Main function
//

int main(int argc, char **argv) {

//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE | GLUT_STENCIL);

	glutInitContextVersion (4, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);

//  Callback Registration
	glutDisplayFunc(renderSim);
	glutReshapeFunc(changeSize);

	glutTimerFunc(0, timer, 0);
	//glutIdleFunc(renderSim);  // Use it for maximum performance
	glutTimerFunc(1000/60, refresh, 0);    //use it to to get 60 FPS whatever

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutKeyboardUpFunc(processKeysUp);
	glutSpecialFunc(SpecialInput);
	glutSpecialUpFunc(SpecialInputUp);

	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc(mouseWheel) ;
	

//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearStencil(0x00);
	glEnable(GL_STENCIL_TEST);

	glClearColor(0.5, 0.6, 0.7, 1.0f);

	printf ("Vendor: %s\n"	, glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n"	, glGetString (GL_VERSION));
	printf ("GLSL: %s\n"	, glGetString (GL_SHADING_LANGUAGE_VERSION));

	/* Initialization of DevIL */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		printf("wrong DevIL version \n");
		exit(0);
	}
	ilInit();

	srand((unsigned int)time(NULL));
	buildScene();

	if(!renderer.setRenderMeshesShaderProg("shaders/mesh_phong.vert", "shaders/mesh_phong.frag") || 
		 !renderer.setRenderTextShaderProg("shaders/ttf.vert", "shaders/ttf.frag"))
	return(1);

	//  GLUT main loop
	glutMainLoop();

	return(0);
}