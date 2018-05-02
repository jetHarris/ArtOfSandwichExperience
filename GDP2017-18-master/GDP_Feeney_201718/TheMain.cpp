// Include glad and GLFW in correct order
#include "globalOpenGL_GLFW.h"


#include <iostream>			// C++ cin, cout, etc.
//#include "linmath.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include <stdlib.h>
#include <stdio.h>
// Add the file stuff library (file stream>
#include <fstream>
#include <sstream>		// "String stream"
#include <string>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <vector>		//  smart array, "array" in most languages
#include "Utilities.h"
#include "ModelUtilities.h"
#include "cMesh.h"
#include "sMeshDrawInfo.h"
#include "cShaderManager.h" 
#include "cGameObject.h"
#include "cVAOMeshManager.h"
#include "cEmitter.h"
#include "cSoundPlayer.h"

#include "cLightManager.h"

// Include all the things that are accessed in other files
#include "globalGameStuff.h"
#include "cCamera.h"
#include "cFBO.h" 
#include "cShaderHolder.h"


cFBO g_FBO_Pass1_G_Buffer;


GLint sexyShaderID;

cShaderHolder* mainShader = NULL;

cGameObject* g_pSkyBoxObject = NULL;	// (theMain.cpp)


// Remember to #include <vector>...
std::vector< cGameObject* >  g_vecGameObjects;

cCamera* g_pTheCamera = NULL;
cCamera* g_pTheSecondCamera = NULL;
cCamera* g_pTheShadowCamera = NULL;

bool splitscreen = false;


extern cGameObject* rayCastedObject;


cVAOMeshManager* g_pVAOManager = 0;		// or NULL, or nullptr

cShaderManager*		g_pShaderManager = 0;		// Heap, new (and delete)
cLightManager*		g_pLightManager = 0;

CTextureManager*		g_pTextureManager = 0;
cSoundPlayer*       g_pSoundManager = 0;

cModelAssetLoader*		g_pModelAssetLoader = NULL;

cEmitter* g_pFlameEmitter = 0;
std::vector< cParticle* > g_FlameParticles;
cGameObject* pParticleObject;

cGameObject* pCuttingObject;

cGameObject* pKnifeObject;

nPhysics::iPhysicsFactory* gPhysicsFactory;
nPhysics::iPhysicsWorld* gPhysicsWorld;
glm::vec3 pastKnifePos;
unsigned int SHADOW_WIDTH = 2000, SHADOW_HEIGHT = 2000;
unsigned int depthMapFBO;
glm::mat4 lightSpaceMatrix;
glm::mat4 lightProjection;
glm::mat4 lightView;
glm::vec3 lightDirection;



#include "cFrameBuffer.h"
#include <ft2build.h>
#include FT_FREETYPE_H


const char* vertexShaderText =
"#version 410\n"
"attribute vec4 coord;"
"varying vec2 texpos;"
"void main () {"
"	gl_Position = vec4(coord.xy, 0, 1);"
"	texpos = coord.zw;"
"}";

const char* fragmentShaderText =
"#version 410\n"
"varying vec2 texpos;"
"uniform bool igcolour;"
"uniform sampler2D tex;"
"uniform vec4 color;"
"void main () {"
" if(igcolour){"
"	gl_FragColor = vec4(texture2D(tex, texpos).xyz, 1);"
"}else{"
"	gl_FragColor = vec4(1, 1, 1, texture2D(tex, texpos).r) * color;"
"	}"
"}";

bool g_IsWindowFullScreen = true;
GLFWwindow* g_pGLFWWindow = NULL;

FT_Library mft;

FT_Face mface;

GLuint vertexShader;
GLuint fragmentShader;
GLuint textProgram;
GLint attribute_coord;
GLint uniform_igcolour;
GLint uniform_tex;
GLint uniform_color;
GLuint mdp_vbo;
GLuint mvao;

extern void mouse_callback(GLFWwindow* window, double xpos, double ypos);
extern void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
extern void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
extern void checkInput();
extern void CreateGoalSandwhiches(bool initialRunThrough);
extern void rotateSandwhichItems();
extern void SetUpParticles();
//void LightFlicker(double curTime);


static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

bool InitPhysics()
{
	gPhysicsFactory = new nPhysics::cBulletPhysicsFactory();
	gPhysicsWorld = gPhysicsFactory->CreateWorld();
	return true;
}

enum RenderSceneTypes
{
	DeferredRenderPass = 0,
	FirstHalfScreen = 1,
	SecondHalfScreen = 2,
	ShadowPass = 3,
	FullScreenOnePlayer = 4
};


int selectedGameObjectIndex = 0;

float totalRunningTime = 0.0f;

bool goingDown = false;
bool goingUp = false;

int playerScore = 0;
float gameTimerInSeconds = 300.0f;
bool gameOver = false;
bool titleScreen = true;
float titleScreenCounter = 15.0f;
bool countDownPlayed = false;
bool titleScreenButtonPressed = false;
bool controlsScreen = false;

const int RENDER_PASS_0_G_BUFFER_PASS = 0;
const int RENDER_PASS_1_DEFERRED_RENDER_PASS = 1;
const int SHADOWPASS = 2;

// Moved to GLFW_keyboardCallback.cpp
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)

GLboolean initfreetype() {

	if (FT_Init_FreeType(&mft))
	{
		fprintf(stderr, "unable to init free type\n");
		return GL_FALSE;
	}


	//if (FT_New_Face(mft, ".\\assets\\fonts\\FreeSans.ttf", 0, &mface))
	if (FT_New_Face(mft, "assets/fonts/Digitalt.ttf", 0, &mface))
	{
		fprintf(stderr, "unable to open font\n");
		return GL_FALSE;
	}

	//set font size
	FT_Set_Pixel_Sizes(mface, 0, 48);


	if (FT_Load_Char(mface, 'X', FT_LOAD_RENDER))
	{
		fprintf(stderr, "unable to load character\n");
		return GL_FALSE;
	}


	return GL_TRUE;
}

GLboolean init_gl() {

	//create shaders
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderText, NULL);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderText, NULL);
	glCompileShader(fragmentShader);

	textProgram = glCreateProgram();
	glAttachShader(textProgram, vertexShader);
	glAttachShader(textProgram, fragmentShader);

	glLinkProgram(textProgram);

	//get vertex attribute/s id/s
	attribute_coord = glGetAttribLocation(textProgram, "coord");
	uniform_igcolour = glGetUniformLocation(textProgram, "igcolour");
	uniform_tex = glGetUniformLocation(textProgram, "tex");
	uniform_color = glGetUniformLocation(textProgram, "color");

	if (attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1 || uniform_igcolour == -1) {
		fprintf(stderr, "unable to get attribute or uniform from shader\n");
		return GL_FALSE;
	}

	//generate and bind vbo 
	glGenBuffers(1, &mdp_vbo);

	//generate and bind vao
	glGenVertexArrays(1, &mvao);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	return GL_TRUE;
}


int main(void)
{
	InitPhysics();

	glfwSetErrorCallback(error_callback);

	srand(time(NULL));
	initfreetype();

	if (!glfwInit())
	{
		std::cout << "ERROR: Couldn't init GLFW, so we're pretty much stuck; do you have OpenGL??" << std::endl;
		return -1;
	}

	int height = 480;	/* default */
	int width = 640;	// default
	std::string title = "Art Of Sandwhich Experience";

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	::g_pGLFWWindow = glfwCreateWindow(width, height,
		title.c_str(),
		NULL, NULL);

	if (!::g_pGLFWWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(::g_pGLFWWindow, key_callback);
	glfwSetCursorPosCallback(::g_pGLFWWindow, mouse_callback);
	glfwSetScrollCallback(::g_pGLFWWindow, scroll_callback);
	glfwSetMouseButtonCallback(::g_pGLFWWindow, mouse_button_callback);
	// For the FBO to resize when the window changes
	glfwSetWindowSizeCallback(::g_pGLFWWindow, window_size_callback);
	glfwSetCursorPos(::g_pGLFWWindow, 4000, 168);

	glfwMakeContextCurrent(::g_pGLFWWindow);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	std::cout << glGetString(GL_VENDOR) << " "
		<< glGetString(GL_RENDERER) << ", "
		<< glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// General error string, used for a number of items during start up
	std::string error;

	::g_pShaderManager = new cShaderManager();
	::mainShader = new cShaderHolder();

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";
	fragShader.fileName = "simpleFragDeferred.glsl";

	::g_pShaderManager->setBasePath("assets//shaders//");

	if (!::g_pShaderManager->createProgramFromFile(
		"mySexyShader", vertShader, fragShader))
	{
		std::cout << "Oh no! All is lost!!! Blame Loki!!!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		return -1;
	}
	std::cout << "The shaders comipled and linked OK" << std::endl;
	init_gl();


	// Load models
	::g_pModelAssetLoader = new cModelAssetLoader();
	::g_pModelAssetLoader->setBasePath("assets/models/");

	::g_pVAOManager = new cVAOMeshManager();

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");


	sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

	if (!Load3DModelsIntoMeshManager(sexyShaderID, ::g_pVAOManager, ::g_pModelAssetLoader, error))
	{
		std::cout << "Not all models were loaded..." << std::endl;
		std::cout << error << std::endl;
	}

	LoadModelsIntoScene();

	::g_pSoundManager = new cSoundPlayer();
	::g_pLightManager = new cLightManager();

	::g_pLightManager->CreateLights(8);

	::g_pLightManager->vecLights[0].setLightParamType(cLight::POINT);
	::g_pLightManager->vecLights[0].position = glm::vec3(10.0f, 30.0f, 0.0f);
	::g_pLightManager->vecLights[0].attenuation.y = 0.02f;

	::g_pLightManager->vecLights[1].setLightParamType(cLight::POINT);
	::g_pLightManager->vecLights[1].position = glm::vec3(10.0f, 30.0f, 40.0f);
	::g_pLightManager->vecLights[1].attenuation.y = 0.013f;

	::g_pLightManager->vecLights[2].setLightParamType(cLight::POINT);
	::g_pLightManager->vecLights[2].position = glm::vec3(10.0f, 20.0f, -40.0f);
	::g_pLightManager->vecLights[2].attenuation.y = 0.013f;

	::g_pLightManager->vecLights[3].setLightParamType(cLight::POINT);
	::g_pLightManager->vecLights[3].position = glm::vec3(-10.77, 10.0f, 14.0f);
	::g_pLightManager->vecLights[3].attenuation.y = 0.05f;

	::g_pLightManager->vecLights[4].setLightParamType(cLight::POINT);
	::g_pLightManager->vecLights[4].position = glm::vec3(-10.77, -10.0f, 14.0f);
	::g_pLightManager->vecLights[4].attenuation.y = 0.05f;

	::g_pLightManager->vecLights[5].setLightParamType(cLight::POINT);
	::g_pLightManager->vecLights[5].position = glm::vec3(-20, 10, 33);
	::g_pLightManager->vecLights[5].attenuation.y = 0.2f;

	::g_pLightManager->vecLights[6].setLightParamType(cLight::POINT);
	::g_pLightManager->vecLights[6].position = glm::vec3(-20, -10, 33);
	::g_pLightManager->vecLights[6].attenuation.y = 0.2f;

	::g_pLightManager->vecLights[7].setLightParamType(cLight::POINT);
	::g_pLightManager->vecLights[7].position = glm::vec3(0.9, 5, 31.4);
	::g_pLightManager->vecLights[7].attenuation.y = 0.001f;
	::g_pLightManager->vecLights[7].attenuation.x = 0.07f;
	::g_pLightManager->vecLights[7].attenuation.z = 0.01f;
	::g_pLightManager->vecLights[7].diffuse = glm::vec3(0.0f, 0.0f, 0.0f);


	::g_pFlameEmitter = new cEmitter(glm::vec3(0.9, 1.8, 31.4));
	::g_pFlameEmitter->init(3500, 300, //max number of particles, max created per step
		glm::vec3(0.0f, 0.3f, 0.0f),	// Min init vel
		glm::vec3(0.0f, 10.0f, 0.0f),	// max init vel
		1.0f, 2.0f);


	::g_pLightManager->LoadShaderUniformLocations(currentProgID);



	// Texture 
	::g_pTextureManager = new CTextureManager();

	std::cout << "GL_MAX_TEXTURE_IMAGE_UNITS: " << ::g_pTextureManager->getOpenGL_GL_MAX_TEXTURE_IMAGE_UNITS() << std::endl;
	std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: " << ::g_pTextureManager->getOpenGL_GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS() << std::endl;

	::g_pTextureManager->setBasePath("assets/textures");

	::g_pTextureManager->Create2DTextureFromBMPFile("KnifeTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("BreadLoafT.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("BreadLoafToastedT.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("BreadSliceTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("CheeseTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("CuttingBoardTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("tomatoTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("MeatTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("OliveTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("PlateTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("WallTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("CeilTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("CounterTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("FrontWallTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("ToasterTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("BreadToasterTexture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("Restaurant.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("cheeseInside.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("tomatoInside.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("breadInside.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("breadToastedInside.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("titleScreen.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("controlsScreen.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("instructionsScreen.bmp", true);


	::g_pTextureManager->setBasePath("assets/textures/skybox");
	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles(
		"space",
		"SpaceBox_right1_posX.bmp",
		"SpaceBox_left2_negX.bmp",
		"SpaceBox_top3_posY.bmp",
		"SpaceBox_bottom4_negY.bmp",
		"SpaceBox_front5_posZ.bmp",
		"SpaceBox_back6_negZ.bmp", true, true))
	{
		std::cout << "Didn't load skybox" << std::endl;
	}

	::g_pTextureManager->setBasePath("assets/textures");
	::g_pTextureManager->Create2DTextureFromBMPFile("test.bmp", true);

	::g_pTheCamera = new cCamera();
	::g_pTheSecondCamera = new cCamera();
	::g_pTheShadowCamera = new cCamera();

	g_pTheShadowCamera->cameraPos = glm::vec3(-10, 80, 20);
	g_pTheShadowCamera->cameraFront = glm::vec3(0.01, -1, -0.1);

	//	::g_pTheCamera->FollowCam->SetOrUpdateTarget(glm::vec3(1.0f));

	CreateGoalSandwhiches(true);

	//pCuttingObject = g_vecGameObjects[1];
	pKnifeObject = ::g_vecGameObjects[1];
	pParticleObject = g_vecGameObjects[2];
	pParticleObject->vecMeshes[0].bDisableBackFaceCulling = true;
	pParticleObject->bDiscardTexture = true;

	//::g_pPhysicsWorld = new cPhysicsWorld();


	// All loaded!
	std::cout << "And we're good to go! Staring the main loop..." << std::endl;

	glEnable(GL_DEPTH);

	mainShader->loadUniforms(sexyShaderID);
	//get all the uniform locations

	/*uniLoc_mView = glGetUniformLocation(sexyShaderID, "mView");
	uniLoc_mProjection = glGetUniformLocation(sexyShaderID, "mProjection");*/
	// Create an FBO
	if (!::g_FBO_Pass1_G_Buffer.init(1920, 1080, error))
	{
		std::cout << "::g_FBO_Pass2_Deferred error: " << error << std::endl;
	}

	//make the fbo for the shadows

	glGenFramebuffers(1, &depthMapFBO);

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//set the values for the shadow pass
	float near_plane = 20.0f, far_plane = 567.5f;
	lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

	glm::vec3 eye = glm::vec3(3.0f, 50.0f, 5.0f);
	glm::vec3 target = glm::vec3(0);
	lightView = glm::lookAt(eye,//eye
		target, //target
		glm::vec3(0.0f, 1.0f, 0.0f)); //up

	lightDirection = glm::normalize(eye - target);
	lightSpaceMatrix = lightProjection * lightView;


	setWindowFullScreenOrWindowed(::g_pGLFWWindow, ::g_IsWindowFullScreen);


	SetUpParticles();

	glfwSetInputMode(g_pGLFWWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Gets the "current" time "tick" or "step"
	double lastTimeStep = glfwGetTime();
	double secondCounter = 1.0;
	int framesPerSecond = 0;
	int frames = 0;

	glfwSetWindowTitle(::g_pGLFWWindow, "ART OF SANDWICH EXPERIENCE");

	// Main game or application loop
	while (!glfwWindowShouldClose(::g_pGLFWWindow))
	{
		// Essentially the "frame time"
		// Now many seconds that have elapsed since we last checked
		double curTime = glfwGetTime();
		double deltaTime = curTime - lastTimeStep;
		lastTimeStep = curTime;



		totalRunningTime += deltaTime;
		::gPhysicsWorld->TimeStep(deltaTime);
		checkInput();

		::g_pFlameEmitter->Update((float)deltaTime);
		g_pSoundManager->UpdateSystem();
		rotateSandwhichItems();



		//to move the knife up and down
		float moveSpeed = 0.1;
		glm::vec3 knifePos = pKnifeObject->getPosition();
		if (goingDown)
		{
			pKnifeObject->setPosition(glm::vec3(knifePos.x, knifePos.y - (moveSpeed * 3), knifePos.z));
			//if the knife has reached the bottom of its allowed position
			if (knifePos.y <= 2)
			{
				goingDown = false;
				goingUp = true;
				//also do the cut
				glm::vec3 testNormal = glm::vec3(0, 0, -1);
				glm::vec3 testPos = glm::vec3(1, 0, -3);
				testPos.z = -knifePos.z;
				pastKnifePos = knifePos;
				int sizeOfVecAtCut = ::g_vecGameObjects.size();
				for (int i = 0; i < sizeOfVecAtCut; ++i)
				{
					if (::g_vecGameObjects[i]->cuttable)
					{
						glm::vec3 testPosTemp = testPos;
						testPosTemp.z += ::g_vecGameObjects[i]->getPosition().z;
						glm::vec3 testNormal = glm::vec3(0, 0, -1);
						if (::g_vecGameObjects[i]->Slice(true, testPosTemp, testNormal))
						{
							g_pSoundManager->PlaySound(0);
							knifePos = pKnifeObject->getPosition();
							knifePos.z += 0.1;
							pKnifeObject->setPosition(knifePos);
						}
						
					}
				}
			}
		}
		else if (goingUp)
		{
			pKnifeObject->setPosition(glm::vec3(knifePos.x, knifePos.y + (moveSpeed * 3), knifePos.z));
			if (knifePos.y >= 5)
			{
				goingUp = false;
			}
		}

		if (titleScreen)
		{
			if (titleScreenButtonPressed || titleScreenCounter > 4.1f)
				titleScreenCounter -= deltaTime;
			//if the counter just got below 3 seconds then play a sound
			if (titleScreenCounter < 0)
			{
				titleScreen = false;
				g_pSoundManager->StartGameMusic();
				controlsScreen = true; 
			}
			if (!countDownPlayed && titleScreenCounter < 3.8f)
			{
				countDownPlayed = true;
				g_pSoundManager->PlaySound(7);
			}
		}
		else
		{
			if (gameTimerInSeconds > 0)
			{
				gameTimerInSeconds -= deltaTime;

				if (gameTimerInSeconds < 0.0f)
				{
					gameOver = true;
					g_pSoundManager->PlaySound(6);
					g_pSoundManager->SwitchToGameOverMusic();
				}
			}
		}


		::g_pShaderManager->useShaderProgram("mySexyShader");
		//shadows
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glUniform1i(mainShader->renderPassNumber_LocID, SHADOWPASS);
		glUniform1f(mainShader->renderPassVertexNum, GL_TRUE);
		glUniform1f(mainShader->shadowStuff, GL_TRUE);

		glUniformMatrix4fv(mainShader->lightSpaceMatrixLocation, 1, GL_FALSE,
			(const GLfloat*)glm::value_ptr(lightSpaceMatrix));
		RenderScene(::g_vecGameObjects, ::g_pGLFWWindow, deltaTime, RenderSceneTypes::ShadowPass);


		//    ___                _             _           ___       _            __   __           
		//   | _ \ ___  _ _   __| | ___  _ _  | |_  ___   / __| ___ | |__  _  _  / _| / _| ___  _ _ 
		//   |   // -_)| ' \ / _` |/ -_)| '_| |  _|/ _ \ | (_ ||___|| '_ \| || ||  _||  _|/ -_)| '_|
		//   |_|_\\___||_||_|\__,_|\___||_|    \__|\___/  \___|     |_.__/ \_,_||_|  |_|  \___||_|  
		//                                                                        
		// In this pass, we render all the geometry to the "G buffer"
		// The lighting is NOT done here. 
		// 
		//FirstPass(deltaTime);
		::g_pShaderManager->useShaderProgram("mySexyShader");

		// Direct everything to the FBO

		glBindFramebuffer(GL_FRAMEBUFFER, g_FBO_Pass1_G_Buffer.ID);
		glUniform1i(mainShader->renderPassNumber_LocID, RENDER_PASS_0_G_BUFFER_PASS);
		glUniform1f(mainShader->renderPassVertexNum, GL_FALSE);
		glUniform1f(mainShader->shadowStuff, GL_FALSE);


		g_FBO_Pass1_G_Buffer.clearBuffers();

		/// Pick a texture unit... 
		unsigned int shadowUnit = 55;
		glActiveTexture(GL_TEXTURE0 + 55);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glUniform1i(mainShader->shadowLocId, shadowUnit);

		if (splitscreen)
		{
			RenderScene(::g_vecGameObjects, ::g_pGLFWWindow, deltaTime, RenderSceneTypes::FirstHalfScreen);
			RenderScene(::g_vecGameObjects, ::g_pGLFWWindow, deltaTime, RenderSceneTypes::SecondHalfScreen);
		}
		else
		{
			RenderScene(::g_vecGameObjects, ::g_pGLFWWindow, deltaTime, RenderSceneTypes::FullScreenOnePlayer);
		}


		//    ___         __                         _   ___                _             ___               
		//   |   \  ___  / _| ___  _ _  _ _  ___  __| | | _ \ ___  _ _   __| | ___  _ _  | _ \ __ _  ___ ___
		//   | |) |/ -_)|  _|/ -_)| '_|| '_|/ -_)/ _` | |   // -_)| ' \ / _` |/ -_)| '_| |  _// _` |(_-<(_-<
		//   |___/ \___||_|  \___||_|  |_|  \___|\__,_| |_|_\\___||_||_|\__,_|\___||_|   |_|  \__,_|/__//__/
		//   
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //switch this to zero and it should render the scene
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		::g_pShaderManager->useShaderProgram("mySexyShader");

		glUniform1i(mainShader->renderPassNumber_LocID, RENDER_PASS_1_DEFERRED_RENDER_PASS);
		glUniform1f(mainShader->renderPassVertexNum, GL_FALSE);

		//uniform sampler2D texFBONormal2D;
		//uniform sampler2D texFBOVertexWorldPos2D;

		//has to be higher than the number of textures
		GLint texFBOColour2DTextureUnitID = 30;

		GLint texFBONormal2DTextureUnitID = 31;

		GLint texFBOWorldPosition2DTextureUnitID = 32;




		// Pick a texture unit... 
		glActiveTexture(GL_TEXTURE0 + texFBOColour2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.colourTexture_0_ID);
		glUniform1i(mainShader->texFBOColour2DLocID, texFBOColour2DTextureUnitID);

		glActiveTexture(GL_TEXTURE0 + texFBONormal2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.normalTexture_1_ID);
		glUniform1i(mainShader->texFBONormal2DLocID, texFBONormal2DTextureUnitID);

		glActiveTexture(GL_TEXTURE0 + texFBOWorldPosition2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.vertexWorldPos_2_ID);
		glUniform1i(mainShader->texFBOWorldPosition2DLocID, texFBOWorldPosition2DTextureUnitID);


		// Set the sampler in the shader to the same texture unit (20)

		glfwGetFramebufferSize(::g_pGLFWWindow, &width, &height);


		glUniform1f(mainShader->screenWidthLocID, (float)width);
		glUniform1f(mainShader->screenHeightLocID, (float)height);


		std::vector< cGameObject* >  vecCopy2ndPass;
		// Push back a SINGLE quad or GIANT triangle that fills the entire screen
		// Here we will use the skybox (as it fills the entire screen)
		vecCopy2ndPass.push_back(::g_pSkyBoxObject);

		RenderScene(vecCopy2ndPass, ::g_pGLFWWindow, deltaTime, RenderSceneTypes::DeferredRenderPass);

		//this does the x ray effect, essientially drawing the object again over the scene so
		//it can be scene through everything
		if (rayCastedObject != NULL && !splitscreen)
		{
			std::vector< cGameObject* >  vecCopy2ndPass2;
			vecCopy2ndPass2.push_back(::rayCastedObject);
			RenderScene(vecCopy2ndPass2, ::g_pGLFWWindow, deltaTime, RenderSceneTypes::DeferredRenderPass);
		}


		secondCounter -= deltaTime;
		frames++;
		if (secondCounter <= 0)
		{
			secondCounter = 1.0;
			framesPerSecond = frames;
			frames = 0;
		}


		//the following commented out blocks are for debug purposes, one to display framerate and the other
		//to display where the ray casted object is

		//std::stringstream ssTitle;
		//std::string titleString = "GAME JAM      Framerate:" + std::to_string(framesPerSecond);

		//if (rayCastedObject != NULL)
		//{
		//	glm::vec3 pos = rayCastedObject->getPosition();
		//	titleString = "GAME JAM      Location: x:" + std::to_string(pos.x) + " y: " + std::to_string(pos.y) + " z:" + std::to_string(pos.z);
		//}

		//glfwSetWindowTitle(::g_pGLFWWindow, titleString.c_str());


		// "Presents" what we've drawn
		// Done once per scene 
		glfwSwapBuffers(::g_pGLFWWindow);
		glfwPollEvents();
	}// while ( ! glfwWindowShouldClose(window) )


	glfwDestroyWindow(::g_pGLFWWindow);
	glfwTerminate();

	// 
	delete ::g_pShaderManager;
	delete ::g_pVAOManager;
	delete ::g_pSoundManager;
	delete ::mainShader;

	//    exit(EXIT_SUCCESS);
	return 0;
}



void setWindowFullScreenOrWindowed(GLFWwindow* pTheWindow, bool IsFullScreen)
{
	// Set full-screen or windowed
	if (::g_IsWindowFullScreen)
	{
		// Find the size of the primary monitor
		GLFWmonitor* pPrimaryScreen = glfwGetPrimaryMonitor();
		const GLFWvidmode* pPrimMonVidMode = glfwGetVideoMode(pPrimaryScreen);
		// Set this window to full screen, matching the size of the monitor:
		glfwSetWindowMonitor(pTheWindow, pPrimaryScreen,
			0, 0,				// left, top corner 
			pPrimMonVidMode->width, pPrimMonVidMode->height,
			GLFW_DONT_CARE);	// refresh rate

		std::cout << "Window now fullscreen at ( "
			<< pPrimMonVidMode->width << " x "
			<< pPrimMonVidMode->height << " )" << std::endl;
	}
	else
	{
		// Make the screen windowed. (i.e. It's CURRENTLY full-screen)
		// NOTE: We aren't saving the "old" windowed size - you might want to do that...
		// HACK: Instead, we are taking the old size and mutiplying it by 75% 
		// (the thinking is: the full-screen switch always assumes we want the maximum
		//	resolution - see code above - but when we make that maximum size windowed,
		//  it's going to be 'too big' for the screen)
		GLFWmonitor* pPrimaryScreen = glfwGetPrimaryMonitor();
		const GLFWvidmode* pPrimMonVidMode = glfwGetVideoMode(pPrimaryScreen);

		// Put the top, left corner 10% of the size of the full-screen
		int topCornerTop = (int)((float)pPrimMonVidMode->height * 0.1f);
		int topCornerLeft = (int)((float)pPrimMonVidMode->width * 0.1f);
		// Make the width and height 75% of the full-screen resolution
		int height = (int)((float)pPrimMonVidMode->height * 0.75f);
		int width = (int)((float)pPrimMonVidMode->width * 0.75f);

		glfwSetWindowMonitor(pTheWindow, NULL,		// This NULL makes the screen windowed
			topCornerLeft, topCornerTop,				// left, top corner 
			width, height,
			GLFW_DONT_CARE);	// refresh rate

		std::cout << "Window now windowed at ( "
			<< width << " x " << height << " )"
			<< " and offset to ( "
			<< topCornerLeft << ", " << topCornerTop << " )"
			<< std::endl;
	}
	return;
}
