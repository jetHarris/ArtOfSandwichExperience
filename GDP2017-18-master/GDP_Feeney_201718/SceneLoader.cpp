// This file is used to laod the models
#include "cGameObject.h"
#include <vector>
#include "Utilities.h"		// getRandInRange()
#include <glm/glm.hpp>
#include "globalGameStuff.h"

// For the cSimpleAssimpSkinnedMeshLoader class
#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

#include "cAnimationState.h"

extern std::vector< cGameObject* >  g_vecGameObjects;
extern cGameObject* g_pTheDebugSphere;
extern nPhysics::iPhysicsFactory* gPhysicsFactory;
extern nPhysics::iPhysicsWorld* gPhysicsWorld;

cGameObject* pPlate1;
cGameObject* pPlate2;
cGameObject* pPlate3;
cGameObject* pToaster;

cGameObject* pFrontWall;
cGameObject* pRightWall;
cGameObject* pLeftWall;
cGameObject* pBackWall;


void LoadModelsIntoScene(void)
{

	{	
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Ground";

		cPhysicalProperties physState;
		physState.position = glm::vec3(0.0f, -4.0f, 0.0f);
		physState.setOrientationEulerAngles(glm::vec3(0, 90, 0), true);
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 80.0f;
		meshInfo.debugDiffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshInfo.name = "plainPlane.ply";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("CounterTexture.bmp", 1.0f));
		
		pTempGO->vecMeshes.push_back(meshInfo);

		nPhysics::iShape* shape;
		glm::vec3 normal = glm::vec3(0, 1, 0);
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 0.0f;
		desc.Position = pTempGO->getPosition();
		shape = gPhysicsFactory->CreatePlane(normal, 1);
		pTempGO->physics = false;
		pTempGO->lookable = false;
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);


		::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add
	}

	//{
	//	cGameObject* pTempGO = new cGameObject();
	//	pTempGO->friendlyName = "meat";
	//	pTempGO->type = ObjectType::SALAMI;

	//	cPhysicalProperties physState;
	//	physState.position = glm::vec3(0.0f, 3.0, -3.0);
	//	physState.setOrientationEulerAngles(glm::vec3(0, 90, 0), true);
	//	pTempGO->SetPhysState(physState);
	//	pTempGO->cuttable = true;

	//	sMeshDrawInfo meshInfo;
	//	meshInfo.scale = 2.0f; //5.0f for zuc
	//	
	//	meshInfo.name = "meat.ply";
	//	meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	//	meshInfo.bDrawAsWireFrame = false;
	//	meshInfo.bDisableBackFaceCulling = true;
	//	meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("MeatTexture.bmp", 1.0f));
	//	meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("MeatTexture.bmp", 0.0f));
	//	
	//	pTempGO->vecMeshes.push_back(meshInfo);
	//	::g_vecGameObjects.push_back(pTempGO);
	//}
	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "knife";
		pTempGO->type = ObjectType::KNIFE;

		cPhysicalProperties physState;
		physState.position = glm::vec3(2.0f, 6.0, 0.0);
		physState.setOrientationEulerAngles(glm::vec3(90, 0, -90), true);
		pTempGO->SetPhysState(physState);

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 0.2f;
		
		meshInfo.name = "knife.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("KnifeTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);
		::g_vecGameObjects.push_back(pTempGO);
	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "SmoothSphere.ply";
		pTempGO->type = ObjectType::KNIFE;

		cPhysicalProperties physState;
		physState.position = glm::vec3(2.0f,-400, 0.0);
		physState.setOrientationEulerAngles(glm::vec3(90, 0, -90), true);
		pTempGO->SetPhysState(physState);

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 0.1f;
		meshInfo.name = "lowPolySphere.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("tomatoInside.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);
		pTempGO->castShadow = false;
		pTempGO->useLighting = false;
		::g_vecGameObjects.push_back(pTempGO);
	}

	

	//Tomato spawner
	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Tomato";
		pTempGO->type = ObjectType::TOMATO;

		cPhysicalProperties physState;
		physState.position = glm::vec3(-14.0f, 3.0, 5.0); 
		physState.setOrientationEulerAngles(glm::vec3(0, 0, 90), true);
		pTempGO->SetPhysState(physState);
		pTempGO->physics = true;
		pTempGO->spawner = true;
		pTempGO->spawnerType = 0;

		

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 2.0f; //5.0f for zuc
		
		meshInfo.name = "tomato.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("tomatoTexture.bmp", 1.0f));
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("tomatoInside.bmp", 0.0f));
		
		pTempGO->vecMeshes.push_back(meshInfo);


		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 10.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

		::g_vecGameObjects.push_back(pTempGO);
	}
	//Cheese spawner
	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Cheese";
		pTempGO->type = ObjectType::CHEESE;

		cPhysicalProperties physState;
		physState.position = glm::vec3(-14.0f, 3.0, -4.0);
		physState.setOrientationEulerAngles(glm::vec3(0, 0, 0), true);
		pTempGO->SetPhysState(physState);
		pTempGO->physics = true;
		pTempGO->spawner = true;
		pTempGO->spawnerType = 1;


		sMeshDrawInfo meshInfo;
		meshInfo.scale = 2.0f; //5.0f for zuc
		
		meshInfo.name = "cheese.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("CheeseTexture.bmp", 1.0f));
		
		pTempGO->vecMeshes.push_back(meshInfo);


		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 10.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

		::g_vecGameObjects.push_back(pTempGO);
	}
	//meat spawner
	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Meat";
		pTempGO->type = ObjectType::SALAMI;

		cPhysicalProperties physState;
		physState.position = glm::vec3(-14.0f, 3.0, 0.0);
		physState.setOrientationEulerAngles(glm::vec3(0, 0, 0), true);
		pTempGO->SetPhysState(physState);
		pTempGO->physics = true;
		pTempGO->spawner = true;
		pTempGO->spawnerType = 2;

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 2.0f; //5.0f for zuc
		
		meshInfo.name = "meat.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("MeatTexture.bmp", 1.0f));
		
		pTempGO->vecMeshes.push_back(meshInfo);


		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 10.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

		::g_vecGameObjects.push_back(pTempGO);
	}
	//bread spawner
	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Bread";
		pTempGO->type = ObjectType::BREADLOAF;

		cPhysicalProperties physState;
		physState.position = glm::vec3(-14.0f, 3.0, 11.0);
		physState.setOrientationEulerAngles(glm::vec3(0, 0, 0), true);
		pTempGO->SetPhysState(physState);
		pTempGO->physics = true;
		pTempGO->spawner = true;
		pTempGO->spawnerType = 3;

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 2.0f;
		
		meshInfo.name = "breadLoaf.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("BreadLoafT.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);

		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 10.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

		::g_vecGameObjects.push_back(pTempGO);
	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "cuttingBoard";

		cPhysicalProperties physState;
		physState.position = glm::vec3(0.0f, 0.0, 0.0);
		physState.setOrientationEulerAngles(glm::vec3(0, 90, 0), true);
		pTempGO->SetPhysState(physState);
		pTempGO->physics = true;
		pTempGO->lookable = false;

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 2.0f; //5.0f for zuc
		
		meshInfo.name = "cuttingBoard.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("CuttingBoardTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);

		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 10.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

		::g_vecGameObjects.push_back(pTempGO);

	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "cuttingBoard";

		cPhysicalProperties physState;
		physState.position = glm::vec3(0.0f, 0.0, 15.0);
		physState.setOrientationEulerAngles(glm::vec3(0, 90, 0), true);
		pTempGO->SetPhysState(physState);
		pTempGO->physics = true;
		pTempGO->lookable = false;

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 2.0f; //5.0f for zuc
		
		meshInfo.name = "cuttingBoard.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("CuttingBoardTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);

		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 10.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

		::g_vecGameObjects.push_back(pTempGO);
	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "cuttingBoard2";

		cPhysicalProperties physState;
		physState.position = glm::vec3(-14.0f, 0.0, 0.0);
		physState.setOrientationEulerAngles(glm::vec3(0, 90, 0), true);
		pTempGO->SetPhysState(physState);
		pTempGO->physics = true;
		pTempGO->lookable = false;

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 2.0f; //5.0f for zuc
		
		meshInfo.name = "cuttingBoard2.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("CuttingBoardTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);

		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 10.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

		::g_vecGameObjects.push_back(pTempGO);

	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "cuttingBoard2";

		cPhysicalProperties physState;
		physState.position = glm::vec3(-14.0f, 0.0, 15.0);
		physState.setOrientationEulerAngles(glm::vec3(0, 90, 0), true);
		pTempGO->SetPhysState(physState);
		pTempGO->physics = true;
		pTempGO->lookable = false;

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 2.0f; //5.0f for zuc
		
		meshInfo.name = "cuttingBoard2.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("CuttingBoardTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);

		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 10.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

		::g_vecGameObjects.push_back(pTempGO);
	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "toaster";

		cPhysicalProperties physState;
		physState.position = glm::vec3(0.9, -2,31.4);
		physState.setOrientationEulerAngles(glm::vec3(0, 0, 0), true);
		pTempGO->SetPhysState(physState);
		pTempGO->physics = true;
		pTempGO->lookable = true;

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 3.0f; //5.0f for zuc
		meshInfo.name = "toaster.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("ToasterTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);

		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 0.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

		pToaster = pTempGO;
		::g_vecGameObjects.push_back(pTempGO);

	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "olive";
		pTempGO->type = ObjectType::OLIVE;

		cPhysicalProperties physState;
		physState.position = glm::vec3(-14.0f, 3.0, 15.0);
		physState.setOrientationEulerAngles(glm::vec3(0, 90, 90), true);
		pTempGO->SetPhysState(physState);
		pTempGO->physics = true;
		pTempGO->lookable = true;

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 0.5f; //5.0f for zuc
		
		meshInfo.name = "olive.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.bDisableBackFaceCulling = true;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("OliveTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);

		pTempGO->spawner = true;
		pTempGO->spawnerType = 4;

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 1.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(0.3,0.3,0.3));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

		::g_vecGameObjects.push_back(pTempGO);

	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "plate";

		cPhysicalProperties physState;
		physState.position = glm::vec3(-32.0f, 10.0, 17.0);
		physState.setOrientationEulerAngles(glm::vec3(0, 0, 0), true);
		pTempGO->SetPhysState(physState);
		pTempGO->castShadow = false;


		sMeshDrawInfo meshInfo;
		meshInfo.scale = 3.3f;
		meshInfo.name = "plate.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("PlateTexture.bmp", 1.0f));
		
		pTempGO->vecMeshes.push_back(meshInfo);

		pPlate1 = pTempGO;
		::g_vecGameObjects.push_back(pTempGO);
	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "plate";

		cPhysicalProperties physState;
		physState.position = glm::vec3(-32.0f, 10.0f, 9.5f);
		physState.setOrientationEulerAngles(glm::vec3(0, 0, 0), true);
		pTempGO->SetPhysState(physState);
		pTempGO->castShadow = false;

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 3.3f;
		meshInfo.name = "plate.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("PlateTexture.bmp", 1.0f));
		
		pTempGO->vecMeshes.push_back(meshInfo);
		pPlate2 = pTempGO;
		::g_vecGameObjects.push_back(pTempGO);
	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "plate";

		cPhysicalProperties physState;
		physState.position = glm::vec3(-32.0f, 10.0f, 2.0f);
		physState.setOrientationEulerAngles(glm::vec3(0, 0, 0), true);
		pTempGO->SetPhysState(physState);
		pTempGO->castShadow = false;

		sMeshDrawInfo meshInfo;
		meshInfo.scale = 3.3f;
		meshInfo.name = "plate.ply";
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		meshInfo.bDrawAsWireFrame = false;
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("PlateTexture.bmp", 1.0f));
		
		pTempGO->vecMeshes.push_back(meshInfo);
		pPlate3 = pTempGO;
		::g_vecGameObjects.push_back(pTempGO);
	}

	
	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Wall";

		cPhysicalProperties physState;
		physState.position = glm::vec3(-39.0f, 18.0f, 10.0f);
		physState.setOrientationEulerAngles(glm::vec3(90, 90, 0), true);
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 15.0f;
		meshInfo.debugDiffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshInfo.name = "plainPlane.ply";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Restaurant.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);
		pTempGO->castShadow = false;
		//pTempGO->useLighting = false;

		pTempGO->physics = false;

		::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add
	}

	//front wall
	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "frontWall.ply";

		cPhysicalProperties physState;
		physState.position = glm::vec3(-32.0f, 15.0f, 0.0f);
		physState.setOrientationEulerAngles(glm::vec3(0, -180, 0), true);
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 3.0f;
		meshInfo.debugDiffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshInfo.name = "frontWall.ply";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("FrontWallTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);
		pTempGO->castShadow = false;
		//pTempGO->useLighting = false;

		pFrontWall = pTempGO;


		//make 4 boxes around the hole
		//bottom box
		{
			nPhysics::iShape* shape;
			nPhysics::sRigidBodyDesc desc;
			desc.Mass = 0.0f;
			desc.Position = glm::vec3(-33.9f, -1, 0);

			shape = gPhysicsFactory->CreateBox(glm::vec3(4, 10, 40));
			pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
			::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		}

		//top box
		{
			nPhysics::iShape* shape;
			nPhysics::sRigidBodyDesc desc;
			desc.Mass = 0.0f;
			desc.Position = glm::vec3(-33.9f, 34, 0);

			shape = gPhysicsFactory->CreateBox(glm::vec3(6, 10, 40));
			pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
			::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		}

		//right box
		{
			nPhysics::iShape* shape;
			nPhysics::sRigidBodyDesc desc;
			desc.Mass = 0.0f;
			desc.Position = glm::vec3(-33.9f, 0, -21.5);

			shape = gPhysicsFactory->CreateBox(glm::vec3(6, 50, 20));
			pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
			::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		}

		//left box
		{
			nPhysics::iShape* shape;
			nPhysics::sRigidBodyDesc desc;
			desc.Mass = 0.0f;
			desc.Position = glm::vec3(-33.9f, 0, 40);

			shape = gPhysicsFactory->CreateBox(glm::vec3(6, 50, 20));
			pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
			::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
		}

		::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add
	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Wall";

		cPhysicalProperties physState;
		physState.position = glm::vec3(0.0f, 15.0f, -20.0f);
		physState.setOrientationEulerAngles(glm::vec3(90, 0, 0), true);
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 40.0f;
		meshInfo.debugDiffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshInfo.name = "plainPlane.ply";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("WallTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);
		pTempGO->castShadow = false;
		//pTempGO->useLighting = false;


		pRightWall = pTempGO;

		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 0.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);

		pTempGO->physics = false;

		::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add
	}

	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Wall";

		cPhysicalProperties physState;
		physState.position = glm::vec3(0.0f, 15.0f, 40.0f);
		physState.setOrientationEulerAngles(glm::vec3(-90, 0, 0), true);
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 40.0f;
		meshInfo.debugDiffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshInfo.name = "plainPlane.ply";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("WallTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);
		pTempGO->castShadow = false;
		//pTempGO->useLighting = false;

		pLeftWall = pTempGO;


		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 0.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);

		pTempGO->physics = false;

		::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add
	}

	//back wall
	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Wall";

		cPhysicalProperties physState;
		physState.position = glm::vec3(38.0f, 15.0f, 0.0f);
		physState.setOrientationEulerAngles(glm::vec3(90, -90, 0), true);
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 40.0f;
		meshInfo.debugDiffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshInfo.name = "plainPlane.ply";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("WallTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);
		pTempGO->castShadow = false;
		//pTempGO->useLighting = false;

		pBackWall = pTempGO;

		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 0.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(zWidth, yWidth, xWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);

		pTempGO->physics = false;

		::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add
	}

	//ceiling
	{
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "ceiling";

		cPhysicalProperties physState;
		physState.position = glm::vec3(0.0f, 50.0f, 20.0f);
		physState.setOrientationEulerAngles(glm::vec3(0, 0, -180), true);
		pTempGO->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 40.0f;
		meshInfo.debugDiffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshInfo.name = "plainPlane.ply";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("CeilTexture.bmp", 1.0f));
		pTempGO->vecMeshes.push_back(meshInfo);
		pTempGO->castShadow = false;

		pTempGO->RecalculateWorldMatrix();
		float xWidth;
		float yWidth;
		float zWidth;
		pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 0.0f;
		desc.Position = pTempGO->getPosition();

		shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, 1, xWidth));
		pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(pTempGO->rBody);

		pTempGO->physics = false;

		::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add
	}

	// Our skybox object
	{
		//cGameObject* pTempGO = new cGameObject();
		::g_pSkyBoxObject = new cGameObject();
		cPhysicalProperties physState;
		::g_pSkyBoxObject->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 1000.0f;
		
		meshInfo.name = "SmoothSphereRadius1InvertedNormals";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("titleScreen.bmp", 1.0f));
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("instructionsScreen.bmp", 0.0f));
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("controlsScreen.bmp", 0.0f));
		meshInfo.vecMeshCubeMaps.push_back(sTextureBindBlendInfo("space", 1.0f));
		meshInfo.bIsSkyBoxObject = true;
		::g_pSkyBoxObject->vecMeshes.push_back(meshInfo);
		// IS SKYBOX
		::g_vecGameObjects.push_back(::g_pSkyBoxObject);		// Fastest way to add
	}

	return;
}
