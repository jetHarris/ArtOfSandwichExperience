#ifndef _globalGameStuff_HG_
#define _globalGameStuff_HG_

// These are files that do NOT require glad or GLFW to be included
// (So be careful what you add here)
//

#include "cGameObject.h"
#include <vector>
#include <glm/vec3.hpp>
#include "cLightManager.h"
#include "cModelAssetLoader.h"
#include "cCamera.h"
#include "Physics/cPhysicsWorld.h"

// ******************************************************************
// These require the inclusion of the OpenGL and-or GLFW headers
#include "cVAOMeshManager.h"
#include "cShaderManager.h"
#include "Texture/CTextureManager.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <BulletPhysics\BulletPhysics\cBulletPhysicsFactory.h>
// ******************************************************************

extern std::vector< cGameObject* >  g_vecGameObjects;

cGameObject* findObjectByFriendlyName( std::string friendlyName, std::vector<cGameObject*> &vec_pGameObjects );
cGameObject* findObjectByUniqueID( unsigned int ID, std::vector<cGameObject*> &vec_pGameObjects );

extern cLightManager*	g_pLightManager;	// (theMain.cpp)
extern cModelAssetLoader* g_pModelAssetLoader;	// (ModelUtilies.cpp)

extern cCamera* g_pTheCamera;		// (theMain.cpp)


class cGameObject;	// Forward declare to avoid include

extern cVAOMeshManager*			g_pVAOManager;		// (theMain.cpp)
extern cShaderManager*			g_pShaderManager;	// (theMain.cpp)
extern CTextureManager*			g_pTextureManager;	// (theMain.cpp)

void RenderScene(std::vector< cGameObject* > &vec_pGOs, GLFWwindow* pGLFWWindow, double deltaTime, int type);

extern cGameObject* g_pSkyBoxObject;	// (theMain.cpp)

extern	GLFWwindow* g_pGLFWWindow;	// In TheMain.cpp
extern bool g_IsWindowFullScreen;	// false at start
void setWindowFullScreenOrWindowed( GLFWwindow* pTheWindow, bool IsFullScreen );	// In TheMain.cpp


#endif
