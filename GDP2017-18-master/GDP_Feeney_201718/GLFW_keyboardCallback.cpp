#include "globalOpenGL_GLFW.h"
#include "globalGameStuff.h"
#include "Input.h"

#include <iostream>
#include <algorithm>

#include "cAnimationState.h"
#include "cEmitter.h"
#include "cSoundPlayer.h"




bool isShiftKeyDown(int mods, bool bByItself = true);
bool isCtrlKeyDown(int mods, bool bByItself = true);
bool isAltKeyDown(int mods, bool bByItself = true);
bool areAllModifierKeysUp(int mods);
bool areAnyModifierKeysDown(int mods);

const float CAMERASPEED = 0.3f;

extern std::string g_AnimationToPlay;
extern int selectedGameObjectIndex;
extern nPhysics::iPhysicsFactory* gPhysicsFactory;
extern nPhysics::iPhysicsWorld* gPhysicsWorld;
extern cEmitter* g_pFlameEmitter;

extern double g_CA_CountDown;// = 0.0f;
extern bool goingDown;
extern bool goingUp;
extern bool gameOver;
extern bool titleScreenButtonPressed;
extern float titleScreenCounter;

extern int playerScore;

float lastX = 400.0f, lastY = 300.0f;
float yaw = 0.0f;
float pitch = 0.0f;

float lastX2 = 5000.0f, lastY2 = 168.0f;
float yaw2 = 0.0f;
float pitch2 = 0.0f;

bool firstMouse = true;

bool firstMouse2 = true;

extern cGameObject* rayCastedObject;
extern cGameObject* rayCastedObject2;
extern cGameObject* pCuttingObject;

extern cGameObject* pPlate1;
extern cGameObject* pPlate2;
extern cGameObject* pPlate3;

extern cGameObject* pToaster;

extern cGameObject* pKnifeObject;

extern cGameObject* pFrontWall;
extern cGameObject* pRightWall;
extern cGameObject* pLeftWall;
extern cGameObject* pBackWall;

extern cSoundPlayer* g_pSoundManager;

int finishCounter = 0;

extern bool splitscreen;
extern bool draggingFirst;
extern bool draggingSecond;
extern float destinationDistance;
extern float destinationDistance2;
extern cCamera* g_pTheSecondCamera;
extern bool titleScreen;

//std::vector<std::vector<cGameObject*>> sandwhiches;

std::vector<cGameObject*> sandwhichDisplayItems;
std::vector<std::vector<std::pair<glm::vec3, cGameObject*>>> goalSandwhiches;

std::vector<cGameObject*> finishedSandwhichItems;

//variables needed so a press isn't detected too many times
bool RBPressed = false;
bool BPressed = false;
bool XPressed = false;

bool RBPressedSecond = false;
bool BPressedSecond = false;
bool XPressedSecond = false;

//a boolean to keep track of if the plate sandwhiches need to be moved away or not
bool moveFinishedSammies = false;

//a boolean for demonstration purposes, where I switch the inside textures of the sliced objects
bool demonstrateCheese = false;

//objects used to show goal sandwiches
cGameObject* oliveObject;
cGameObject* breadSliceObject;
cGameObject* breadSliceToastedObject;
cGameObject* meatObject;
cGameObject* cheeseObject;
cGameObject* tomatoObject;

void ChangeHoldDistance(int type, double change);

//method to spawn a new object, used when a spawner object is clicked
cGameObject* createNewObject(std::string modelName, std::string textureName, glm::vec3& pos, ObjectType type)
{
	cGameObject* pTempGO = new cGameObject();
	cPhysicalProperties physState;
	physState.position = pos;
	physState.setOrientationEulerAngles(glm::vec3(0, 0, 0), true);
	pTempGO->SetPhysState(physState);
	pTempGO->castShadow = false;
	sMeshDrawInfo meshInfo;
	meshInfo.scale = 1.0f;
	meshInfo.name = modelName;
	meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	meshInfo.bDrawAsWireFrame = false;
	meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo(textureName, 1.0f));
	pTempGO->vecMeshes.push_back(meshInfo);
	pTempGO->type = type;
	::g_vecGameObjects.push_back(pTempGO);

	pos.y -= 2.7;
	return pTempGO;
}

void rotateSandwhichItems()
{
	int allSandwhichSize = sandwhichDisplayItems.size();
	for (int i = 0; i < allSandwhichSize; ++i)
	{
		sandwhichDisplayItems[i]->adjQOrientationEuler(glm::vec3(0, -1, 0));
	}
}

void CreateGoalSandwhiches(bool initialRunThrough)
{
	if (initialRunThrough)
	{
		//create all the display items
		glm::vec3 displayDestination = glm::vec3(-44.0f, 46.0, 18.0);
		sandwhichDisplayItems.push_back(createNewObject("olive.ply", "OliveTexture.bmp", displayDestination, ObjectType::OLIVE));
		oliveObject = ::g_vecGameObjects.back();
		sandwhichDisplayItems.push_back(createNewObject("breadSlice.ply", "BreadSliceTexture.bmp", displayDestination, ObjectType::BREADSLICE));
		breadSliceObject = g_vecGameObjects.back();
		sandwhichDisplayItems.push_back(createNewObject("breadSlice.ply", "BreadToasterTexture.bmp", displayDestination, ObjectType::TOASTEDBREADSLICE));
		breadSliceToastedObject = g_vecGameObjects.back();
		sandwhichDisplayItems.push_back(createNewObject("meat.ply", "MeatTexture.bmp", displayDestination, ObjectType::SALAMI));
		meatObject = g_vecGameObjects.back();
		sandwhichDisplayItems.push_back(createNewObject("cheese.ply", "CheeseTexture.bmp", displayDestination, ObjectType::CHEESE));
		cheeseObject = g_vecGameObjects.back();
		sandwhichDisplayItems.push_back(createNewObject("tomato.ply", "tomatoTexture.bmp", displayDestination, ObjectType::TOMATO));
		tomatoObject = g_vecGameObjects.back();
	}

	//there will be 3 goal sandwhiches
	for (int i = 0; i < 3; ++i)
	{
		glm::vec3 newObjectDestination = glm::vec3(-24.0f, 46.0, 18.0 - i * 8);
		std::vector<std::pair<glm::vec3, cGameObject*>> currentSandwhich;

		int toastedNum = (rand() % 2);
		bool toasted = toastedNum == 1;

		//the sandwhiches will start with an olive and a piece of bread
		currentSandwhich.push_back(std::make_pair(newObjectDestination, oliveObject));
		newObjectDestination.y -= 2.7;

		if (!toasted)
			currentSandwhich.push_back(std::make_pair(newObjectDestination, breadSliceObject));
		else
			currentSandwhich.push_back(std::make_pair(newObjectDestination, breadSliceToastedObject));

		newObjectDestination.y -= 2.7;
		//there will be a random number of innards
		int numOfInnards = (rand() % 3) + 2;
		//int numOfInnards = 1;
		for (int j = 0; j < numOfInnards; ++j)
		{
			int randType = (rand() % 3);
			switch (randType)
			{
			case 0:
				currentSandwhich.push_back(std::make_pair(newObjectDestination, meatObject));
				break;
			case 1:
				currentSandwhich.push_back(std::make_pair(newObjectDestination, cheeseObject));
				break;
			case 2:
				currentSandwhich.push_back(std::make_pair(newObjectDestination, tomatoObject));
				break;
			}
			newObjectDestination.y -= 2.7;
		}
		//the sandwhich will end with a piece of bread
		if (!toasted)
			currentSandwhich.push_back(std::make_pair(newObjectDestination, breadSliceObject));
		else
			currentSandwhich.push_back(std::make_pair(newObjectDestination, breadSliceToastedObject));
		goalSandwhiches.push_back(currentSandwhich);
	}
}

bool sortingFunction(cGameObject* first, cGameObject* second)
{
	return first->getPosition().y > second->getPosition().y;
}

void FinishSandwhich(int type)
{
	if (finishCounter < 3)
	{
		//start the sandwhich finishing process using the rayCastedObject
		//starting rayCastingPosition


		cGameObject* oliveObject;
		if (type == 2)
			oliveObject = rayCastedObject2;
		else
			oliveObject = rayCastedObject;

		if (oliveObject == NULL || oliveObject->type != ObjectType::OLIVE)
			return;

		glm::vec3 rayCastStart;
		std::vector<cGameObject*> sandwhich;
		rayCastStart = oliveObject->getPosition();

		sandwhich.push_back(oliveObject);
		oliveObject->physics = false;
		oliveObject->lookable = false;
		oliveObject->isLookedAt = false;
		gPhysicsWorld->RemoveRigidBody(oliveObject->rBody);

		//rayCastStart.y -= 1;
		glm::vec3 rayCastEnd = rayCastStart;
		rayCastEnd.y -= 80;
		bool rayCasted = true;
		int objectsSize = g_vecGameObjects.size();



		while (rayCasted == true)
		{

			rayCasted = false;
			gPhysicsWorld->RayCast(rayCastStart, rayCastEnd);
			//go through all of the objects, see its being hit by this ray
			//if it is, remove the physics from it
			for (int i = 0; i < objectsSize; ++i)
			{
				cGameObject* pTheGO = g_vecGameObjects[i];
				bool ObjectOkToBeRemoved;
				ObjectOkToBeRemoved = pTheGO != oliveObject && pTheGO->physics && pTheGO->lookable && !pTheGO->spawner;

				if (ObjectOkToBeRemoved)
				{
					pTheGO->rBody->GetRayCasted(rayCasted);
					if (rayCasted == true)
					{
						sandwhich.push_back(pTheGO);
						pTheGO->physics = false;
						pTheGO->lookable = false;

						gPhysicsWorld->RemoveRigidBody(pTheGO->rBody);

						break;
					}
				}
			}
		}

		//this is where to go through all the objects in the scene to test if they are inside the sandwhich
		//as well
		if (sandwhich.size() > 2) //a sandwhich less that this will already not be a valid sandwhich
		{
			int allObjectsSize = ::g_vecGameObjects.size();
			glm::vec3 topLoc = sandwhich[1]->getPosition();
			glm::vec3 bottomLoc = sandwhich.back()->getPosition();
			for (int i = 0; i < allObjectsSize; ++i)
			{
				if (::g_vecGameObjects[i]->physics && !::g_vecGameObjects[i]->spawner && ::g_vecGameObjects[i]->lookable)
				{
					glm::vec3 currentPos = ::g_vecGameObjects[i]->getPosition();
					if (abs(topLoc.x - currentPos.x) < 5 && abs(topLoc.z - currentPos.z) < 5
						&& currentPos.y < topLoc.y && currentPos.y > bottomLoc.y)
					{
						sandwhich.push_back(::g_vecGameObjects[i]);
						::g_vecGameObjects[i]->physics = false;
						::g_vecGameObjects[i]->lookable = false;
						gPhysicsWorld->RemoveRigidBody(::g_vecGameObjects[i]->rBody);
					}
				}
			}
			//then sort the vector by height
			std::sort(sandwhich.begin(), sandwhich.end(), sortingFunction);
		}

		//go through the sandwhich changing the locations
		//this is also where we would validate a sandwhich
		bool valid = false;
		int allSandwhichSize = goalSandwhiches.size();
		for (int i = 0; i < allSandwhichSize; ++i)
		{
			int sandwhichSize = goalSandwhiches[i].size();
			if (sandwhich.size() != sandwhichSize)
				continue;
			bool tempValid = true;
			for (int j = 0; j < sandwhichSize; ++j)
			{
				//go through all of the objects comparing type, if a type doesnt match, then break out of the loop
				if (sandwhich[j]->type != goalSandwhiches[i][j].second->type)
				{
					tempValid = false;
					break;
				}
			}
			if (tempValid)
			{
				goalSandwhiches[i].clear();
				valid = true;

				if (moveFinishedSammies)
				{
					moveFinishedSammies = false;
					int finishedSize = finishedSandwhichItems.size();
					for (int j = 0; j < finishedSize; ++j)
					{
						finishedSandwhichItems[j]->setPosition(glm::vec3(0, -400, 0));
					}
				}
				break;
			}
		}

		if (valid)
		{
			playerScore += sandwhich.size() * 10;
			if (sandwhich[1]->type == ObjectType::TOASTEDBREADSLICE)
			{
				playerScore += 10;
			}
			//validate the sandwhich before continuing
			int sandwhichSize = sandwhich.size() - 1;

			cGameObject* thePlate;
			switch (finishCounter)
			{
			case 0:
				thePlate = pPlate1;
				break;
			case 1:
				thePlate = pPlate2;
				break;
			case 2:
				thePlate = pPlate3;
				break;
			}

			finishCounter++;

			if (finishCounter == 3)
			{
				finishCounter = 0;
				goalSandwhiches.clear();
				CreateGoalSandwhiches(false);
				moveFinishedSammies = true;
			}

			sandwhich.front()->overwriteQOrientation(glm::quat());
			glm::vec3 topBreadPosition = sandwhich[1]->getPosition();
			topBreadPosition.y += 1;
			sandwhich.front()->setPosition(topBreadPosition);
			glm::vec3 offset = thePlate->getPosition() - sandwhich.back()->getPosition();
			offset.y += 0.2;
			//the following vec2s will be used for additional scoring based on position of axis
			glm::vec2 minXZ = glm::vec2(topBreadPosition.x, topBreadPosition.z);
			glm::vec2 maxXZ = glm::vec2(topBreadPosition.x, topBreadPosition.z);

			for (int i = sandwhichSize; i >= 0; --i)
			{
				cGameObject* pTheGO = sandwhich[i];
				glm::vec3 position = pTheGO->getPosition();

				//getting the min and max x and z locations that are used for scoring
				if (position.x < minXZ.x)
				{
					minXZ.x = position.x;
				}

				if (position.z < minXZ.y)
				{
					minXZ.y = position.y;
				}

				if (position.x > maxXZ.x)
				{
					maxXZ.x = position.x;
				}

				if (position.z > maxXZ.y)
				{
					maxXZ.y = position.y;
				}

				position.x += offset.x;
				position.y += offset.y;
				position.z += offset.z;
				pTheGO->castShadow = false;
				pTheGO->setPosition(position);
				finishedSandwhichItems.push_back(pTheGO);
			}
			//test the seperation of axis for scoring
			float axisSeperation = glm::distance(minXZ, maxXZ);
			if (axisSeperation < 15)
			{
				playerScore += (15 - axisSeperation);
			}



			//play success music here
			g_pSoundManager->PlaySound(1);
		}
		else
		{
			//play fail music here
			g_pSoundManager->PlaySound(2);
			//go through all items of the sandwhich giving their physics bodies back
			int sandwhichSize = sandwhich.size();
			for (int i = 0; i < sandwhichSize; ++i)
			{
				sandwhich[i]->setPosition(glm::vec3(0, -400, 0));
			}

		}
	}
}

void DragOrSpawn(int type)
{
	bool ObjectNull;
	if (type == 2)
		ObjectNull = rayCastedObject2 != NULL;
	else
		ObjectNull = rayCastedObject != NULL;

	if (ObjectNull)
	{

		//here is where you check if its a spawning object or not
		//if it is a spawning object simply spawn the item
		//and move the current cutting item away
		bool ObjectSpawner;
		if (type == 2)
			ObjectSpawner = rayCastedObject2->spawner;
		else
			ObjectSpawner = rayCastedObject->spawner;

		if (ObjectSpawner)
		{
			g_pSoundManager->PlaySound(3);
			//move the current cutting object
			int spawnerType;
			if (type == 2)
				spawnerType = rayCastedObject2->spawnerType;
			else
				spawnerType = rayCastedObject->spawnerType;

			switch (spawnerType)
			{
			case 0:
			{
				cGameObject* pTempGO = new cGameObject();
				pTempGO->friendlyName = "Tomato";
				pTempGO->type = ObjectType::TOMATO;

				cPhysicalProperties physState;
				physState.position = glm::vec3(0.0f, 3.0, -3.0);
				physState.setOrientationEulerAngles(glm::vec3(0, 90, 0), true);
				pTempGO->SetPhysState(physState);
				pTempGO->cuttable = true;

				sMeshDrawInfo meshInfo;
				meshInfo.scale = 2.0f; //5.0f for zuc
				meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				meshInfo.name = "tomato.ply";
				meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
				meshInfo.bDrawAsWireFrame = false;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("tomatoTexture.bmp", 1.0f));
				if (!demonstrateCheese)
					meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("tomatoInside.bmp", 0.0f));
				else
					meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("cheeseInside.bmp", 0.0f));
				pTempGO->vecMeshes.push_back(meshInfo);
				if (pCuttingObject != NULL)
				{
					pCuttingObject->cuttable = false;
					pCuttingObject->setPosition(glm::vec3(0, -1000, 0));
				}
				pCuttingObject = pTempGO;
				::g_vecGameObjects.push_back(pTempGO);

			}
			break;
			case 1:
			{
				cGameObject* pTempGO = new cGameObject();
				pTempGO->friendlyName = "Cheese";
				pTempGO->type = ObjectType::CHEESE;

				cPhysicalProperties physState;
				physState.position = glm::vec3(0.0f, 3.0, -3.0); //used to be 3 z
				physState.setOrientationEulerAngles(glm::vec3(0, 90, 0), true);
				pTempGO->SetPhysState(physState);
				pTempGO->cuttable = true;

				sMeshDrawInfo meshInfo;
				meshInfo.scale = 2.0f;
				meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				meshInfo.name = "cheese.ply";
				meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
				meshInfo.bDrawAsWireFrame = false;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("CheeseTexture.bmp", 1.0f));
				if (!demonstrateCheese)
					meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("cheeseInside.bmp", 0.0f));
				else
					meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("MeatTexture.bmp", 0.0f));
				pTempGO->vecMeshes.push_back(meshInfo);
				if (pCuttingObject != NULL)
				{
					pCuttingObject->cuttable = false;
					pCuttingObject->setPosition(glm::vec3(0, -1000, 0));
				}
				pCuttingObject = pTempGO;
				::g_vecGameObjects.push_back(pTempGO);
			}
			break;
			case 2:
			{
				cGameObject* pTempGO = new cGameObject();
				pTempGO->friendlyName = "Meat";
				pTempGO->type = ObjectType::SALAMI;

				cPhysicalProperties physState;
				physState.position = glm::vec3(0.0f, 3.0, -3.0);
				physState.setOrientationEulerAngles(glm::vec3(0, 90, 0), true);
				pTempGO->SetPhysState(physState);
				pTempGO->cuttable = true;

				sMeshDrawInfo meshInfo;
				meshInfo.scale = 2.0f; //5.0f for zuc
				meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				meshInfo.name = "meat.ply";
				meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
				meshInfo.bDrawAsWireFrame = false;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("MeatTexture.bmp", 1.0f));
				if (!demonstrateCheese)
					meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("MeatTexture.bmp", 0.0f));
				else 
					meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("cheeseInside.bmp", 0.0f));
				//meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 0.0f));
				pTempGO->vecMeshes.push_back(meshInfo);
				if (pCuttingObject != NULL)
				{
					pCuttingObject->cuttable = false;
					pCuttingObject->setPosition(glm::vec3(0, -1000, 0));
				}
				pCuttingObject = pTempGO;
				::g_vecGameObjects.push_back(pTempGO);
			}
			break;
			case 3:
			{
				//the commented out section was for when the bread was spawned instead of sliced
				//cGameObject* pTempGO = new cGameObject();
				//pTempGO->friendlyName = "Bread";
				//pTempGO->type = ObjectType::BREADSLICE;

				//cPhysicalProperties physState;
				//physState.position = glm::vec3(0.0f, 3.0, 14.0);
				//physState.setOrientationEulerAngles(glm::vec3(0, 0, 90), true);
				//pTempGO->SetPhysState(physState);
				//pTempGO->physics = true;

				//sMeshDrawInfo meshInfo;
				//meshInfo.scale = 2.0f; //5.0f for zuc
				//meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				//meshInfo.name = "breadSlice.ply";
				//meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
				//meshInfo.bDrawAsWireFrame = false;
				//meshInfo.bDisableBackFaceCulling = true;
				//meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("BreadSliceTexture.bmp", 1.0f));
				////meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 0.0f));
				//pTempGO->vecMeshes.push_back(meshInfo);

				//nPhysics::iShape* shape;
				//nPhysics::sRigidBodyDesc desc;
				//desc.Mass = 1.0f;
				//desc.Position = pTempGO->getPosition();

				//pTempGO->RecalculateWorldMatrix();
				//float xWidth;
				//float yWidth;
				//float zWidth;
				//pTempGO->calcXYZDepths(xWidth, yWidth, zWidth);

				//shape = gPhysicsFactory->CreateBox(glm::vec3(xWidth, yWidth, zWidth));
				//pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
				//::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
				//pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

				//::g_vecGameObjects.push_back(pTempGO);

				cGameObject* pTempGO = new cGameObject();
				pTempGO->friendlyName = "bread";
				pTempGO->type = ObjectType::BREADSLICE;

				cPhysicalProperties physState;
				physState.position = glm::vec3(0.0f, 3.0, -3.0);
				physState.setOrientationEulerAngles(glm::vec3(0, 90, 0), true);
				pTempGO->SetPhysState(physState);
				pTempGO->cuttable = true;

				sMeshDrawInfo meshInfo;
				meshInfo.scale = 2.0f;
				meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				meshInfo.name = "breadLoaf.ply";
				meshInfo.bDrawAsWireFrame = false;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("BreadLoafT.bmp", 1.0f));
				if (!demonstrateCheese)
					meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("breadInside.bmp", 0.0f));
				else
					meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("MeatTexture.bmp", 0.0f));
				pTempGO->vecMeshes.push_back(meshInfo);
				if (pCuttingObject != NULL)
				{
					pCuttingObject->cuttable = false;
					pCuttingObject->setPosition(glm::vec3(0, -1000, 0));
				}
				pCuttingObject = pTempGO;
				::g_vecGameObjects.push_back(pTempGO);
			}
			break;
			case 4:
			{

				cGameObject* pTempGO = new cGameObject();
				pTempGO->friendlyName = "olive";
				pTempGO->type = ObjectType::OLIVE;

				cPhysicalProperties physState;
				physState.position = glm::vec3(0.0f, 3.0, 20.0);
				physState.setOrientationEulerAngles(glm::vec3(0, 90, 0), true);
				pTempGO->SetPhysState(physState);
				pTempGO->physics = true;
				pTempGO->lookable = true;

				sMeshDrawInfo meshInfo;
				meshInfo.scale = 0.5f; //5.0f for zuc
				meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				meshInfo.name = "olive.ply";
				meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
				meshInfo.bDrawAsWireFrame = false;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("OliveTexture.bmp", 1.0f));
				pTempGO->vecMeshes.push_back(meshInfo);

				nPhysics::iShape* shape;
				nPhysics::sRigidBodyDesc desc;
				desc.Mass = 0.5f;
				desc.Position = pTempGO->getPosition();

				shape = gPhysicsFactory->CreateSphere(0.5);
				pTempGO->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
				::gPhysicsWorld->AddRigidBody(pTempGO->rBody);
				pTempGO->rBodyOrientationOffset = pTempGO->getQOrientation();

				::g_vecGameObjects.push_back(pTempGO);
			}
			break;
			}
		}
		else//otherwise simply start dragging the object
		{
			g_pSoundManager->PlaySound(4);
			//calculate a new destination distance based on the current distance from the camera to 
			//the object
			cGameObject* rayObject;
			if (type == 2)
			{
				destinationDistance2 = glm::distance(g_pTheSecondCamera->cameraPos, rayCastedObject2->getPosition());
				draggingSecond = true;
				rayObject = rayCastedObject2;

			}
			else
			{
				destinationDistance = glm::distance(g_pTheCamera->cameraPos, rayCastedObject->getPosition());
				draggingFirst = true;
				rayObject = rayCastedObject;
			}

			if (rayObject == pToaster)
			{
				g_pFlameEmitter->emitting = !g_pFlameEmitter->emitting;
				if (g_pFlameEmitter->emitting)
				{
					g_pSoundManager->PlaySound(8);
					::g_pLightManager->vecLights[7].diffuse = glm::vec3(1.0f, 0.1f, 0.1f);
				}
				else
				{
					g_pSoundManager->PlaySound(10);
					::g_pLightManager->vecLights[7].diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
				}
			}

		}
	}
}

void UpdateCamera(double xpos, double ypos, int type)
{
	if (type == 2)
	{
		if (firstMouse2)
		{
			//lastX2 = xpos;
			//lastY2 = ypos;
			firstMouse2 = false;
			xpos += 3700;
		}

		float xoffset = xpos - lastX2;
		float yoffset = lastY2 - ypos;
		lastX2 = xpos;
		lastY2 = ypos;

		float sensitivity = 0.05;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw2 += xoffset;
		pitch2 += yoffset;

		if (pitch2 > 89.0f)
			pitch2 = 89.0f;
		if (pitch2 < -89.0f)
			pitch2 = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw2)) * cos(glm::radians(pitch2));
		front.y = sin(glm::radians(pitch2));
		front.z = sin(glm::radians(yaw2)) * cos(glm::radians(pitch2));
		g_pTheSecondCamera->cameraFront = glm::normalize(front);

		//here is where we want to update the distance based on the camera
		if (yoffset < 0)
		{
			ChangeHoldDistance(2, -0.1);
		}
	}
	else
	{
		if (firstMouse)
		{
			int firstPresent = glfwJoystickPresent(GLFW_JOYSTICK_1);
			if (!splitscreen && firstPresent)
			{
				firstMouse = false;
				xpos += 3700;
			}
			else
			{
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
				glfwSetCursorPos(::g_pGLFWWindow, 5000, 168);
				return;
			}

		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.05;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		g_pTheCamera->cameraFront = glm::normalize(front);

		//here is where we want to update the distance based on the camera
		if (yoffset < 0)
		{
			ChangeHoldDistance(1, -0.1);
		}
	}
}

void MoveKnife(int type)
{
	if (!goingDown && !goingUp)
	{
		if (type == 1)
		{
			glm::vec3 knifePos = pKnifeObject->getPosition();
			if (knifePos.z <= 5)
			{
				pKnifeObject->setPosition(glm::vec3(knifePos.x, knifePos.y, knifePos.z + 0.1));
			}
		}
		else
		{
			glm::vec3 knifePos = pKnifeObject->getPosition();
			if (knifePos.z >= -10.0f)
			{
				pKnifeObject->setPosition(glm::vec3(knifePos.x, knifePos.y, knifePos.z - 0.1));
			}
		}
		g_pSoundManager->PlaySound(9);
	}
}

void PutCameraBackInPlace(cCamera* theCam)
{
	//check that the x position is in the right place
	if (theCam->cameraPos.x < pFrontWall->getPosition().x + 4)
	{
		theCam->cameraPos.x = pFrontWall->getPosition().x  + 4;
	}
	else if (theCam->cameraPos.x > pBackWall->getPosition().x - 4)
	{
		theCam->cameraPos.x = pBackWall->getPosition().x -4;
	}

	if (theCam->cameraPos.z > pLeftWall->getPosition().z -4)
	{
		theCam->cameraPos.z = pLeftWall->getPosition().z -4;
	}
	else if (theCam->cameraPos.z < pRightWall->getPosition().z +4)
	{
		theCam->cameraPos.z = pRightWall->getPosition().z +4;
	}
}

void ChangeHoldDistance(int type, double change)
{
	if (type == 2)
	{
		destinationDistance2 += change;
	}
	else
	{
		destinationDistance += change;
	}
}

void UpdateRotVelRayCastObject(int type, float change)
{
	if (type == 2)
	{
		if (rayCastedObject2 != NULL)
		{
			rayCastedObject2->rBody->SetRotationalVel(glm::vec3(change, 0, 0));
		}
	}
	else
	{
		if (rayCastedObject != NULL)
		{
			rayCastedObject->rBody->SetRotationalVel(glm::vec3(change, 0, 0));
		}
	}
}

void checkInput()
{
	if (gameOver)
		return;

	//if (Input::IsKeyPressed::LeftShift() == true)
	//{
	//	if (rayCastedObject != NULL)
	//	{
	//		rayCastedObject->rBody->ZeroOutRotationalVel();
	//	}
	//}
	if (Input::IsKeyPressed::A() == true)
	{
		glm::vec3 pushValue = glm::normalize(glm::cross((g_pTheCamera->cameraFront), glm::vec3(0, 1, 0))) * CAMERASPEED;

		pushValue.y = 0;
		g_pTheCamera->cameraPos -= pushValue;
		PutCameraBackInPlace(g_pTheCamera);
	}

	if (Input::IsKeyPressed::D() == true)
	{
		glm::vec3 pushValue = glm::normalize(glm::cross((g_pTheCamera->cameraFront), glm::vec3(0, 1, 0))) * CAMERASPEED;

		pushValue.y = 0;
		g_pTheCamera->cameraPos += pushValue;
		PutCameraBackInPlace(g_pTheCamera);
	}

	if (Input::IsKeyPressed::W() == true)
	{
		glm::vec3 pushValue = glm::normalize(g_pTheCamera->cameraFront) * CAMERASPEED;
		pushValue.y = 0;
		g_pTheCamera->cameraPos += pushValue;
		PutCameraBackInPlace(g_pTheCamera);
	}

	if (Input::IsKeyPressed::S() == true)
	{
		glm::vec3 pushValue = glm::normalize(-g_pTheCamera->cameraFront) * CAMERASPEED;
		pushValue.y = 0;
		g_pTheCamera->cameraPos += pushValue;
		PutCameraBackInPlace(g_pTheCamera);

	}

	if (Input::IsKeyPressed::Q() == true)
	{
		MoveKnife(1);
	}
	if (Input::IsKeyPressed::E() == true)
	{
		MoveKnife(2);
	}

	//detecting controller input
	//first controller
	if (splitscreen)
	{
		int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
		if (present == 1)
		{
			int countAxes;
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &countAxes);

			if (axes[0] != 0)
			{
				glm::vec3 pushValue = glm::normalize(glm::cross((g_pTheSecondCamera->cameraFront), glm::vec3(0, 1, 0))) * CAMERASPEED * axes[0];
				pushValue.y = 0;
				g_pTheSecondCamera->cameraPos += pushValue;
				PutCameraBackInPlace(g_pTheSecondCamera);
			}
			if (axes[1] != 0)
			{
				glm::vec3 pushValue = glm::normalize(g_pTheSecondCamera->cameraFront) * CAMERASPEED * axes[1];
				pushValue.y = 0;
				g_pTheSecondCamera->cameraPos += pushValue;
				PutCameraBackInPlace(g_pTheSecondCamera);
			}
			UpdateCamera(lastX2 + (axes[2] * 50.0f), lastY2 + (axes[3] * -50.0f), 2);

			//the triggers for moving the knife
			if (axes[4] != -1)
			{
				MoveKnife(1);
			}

			if (axes[5] != -1)
			{
				MoveKnife(2);
			}

			int countButtons;
			const unsigned char* axesButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &countButtons);

			//if (axesButtons[0] == GLFW_PRESS) //A button
			//{
			//	if (rayCastedObject2 != NULL)
			//	{
			//		rayCastedObject2->rBody->ZeroOutRotationalVel();
			//	}
			//}

			if (axesButtons[10] == GLFW_PRESS) //up on d pad
			{
				ChangeHoldDistance(2, 1);
			}
			if (axesButtons[11] == GLFW_PRESS)
			{
				int type = 0;
				if (splitscreen)
					type = 2;
				UpdateRotVelRayCastObject(type, 5);
			}
			if (axesButtons[12] == GLFW_PRESS) //down on d pad
			{
				ChangeHoldDistance(2, -1);
			}

			if (axesButtons[13] == GLFW_PRESS)
			{
				int type = 0;
				if (splitscreen)
					type = 2;
				UpdateRotVelRayCastObject(type, -5);
			}


			if (RBPressedSecond == false && axesButtons[5] == GLFW_PRESS)
			{
				RBPressedSecond = true;
				DragOrSpawn(2);
			}
			else if (axesButtons[5] == GLFW_RELEASE)
			{
				RBPressedSecond = false;
				if (draggingSecond)
					g_pSoundManager->PlaySound(4);
				draggingSecond = false;
				rayCastedObject2 = NULL;

			}

			if (BPressedSecond == false && axesButtons[1] == GLFW_PRESS)
			{
				BPressedSecond = true;
				if (!goingUp)
					goingDown = true;
			}
			else if (axesButtons[1] == GLFW_RELEASE)
			{
				BPressedSecond = false;
			}

			if (XPressedSecond == false && axesButtons[2] == GLFW_PRESS)
			{
				XPressedSecond = true;
				FinishSandwhich(2);
			}
			else if (axesButtons[2] == GLFW_RELEASE)
			{
				XPressedSecond = false;
			}

		}
	}

	//controller 2 which will be for the top screen
	int present;
	if (splitscreen)
		present = glfwJoystickPresent(GLFW_JOYSTICK_2);
	else
		present = glfwJoystickPresent(GLFW_JOYSTICK_1);

	if (present == 1)
	{
		int countAxes;
		const float* axes;

		if (splitscreen)
			axes = glfwGetJoystickAxes(GLFW_JOYSTICK_2, &countAxes);
		else
			axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &countAxes);

		if (axes[0] != 0)
		{
			glm::vec3 pushValue = glm::normalize(glm::cross((g_pTheCamera->cameraFront), glm::vec3(0, 1, 0))) * CAMERASPEED * axes[0];
			pushValue.y = 0;
			g_pTheCamera->cameraPos += pushValue;
			PutCameraBackInPlace(g_pTheCamera);
		}
		if (axes[1] != 0)
		{
			glm::vec3 pushValue = glm::normalize(g_pTheCamera->cameraFront) * CAMERASPEED * axes[1];
			pushValue.y = 0;
			g_pTheCamera->cameraPos += pushValue;
			PutCameraBackInPlace(g_pTheCamera);
		}

		UpdateCamera(lastX + (axes[2] * 50.0f), lastY + (axes[3] * -50.0f), 0);

		//the triggers for moving the knife
		if (axes[4] != -1)
		{
			MoveKnife(1);
		}

		if (axes[5] != -1)
		{
			MoveKnife(2);
		}

		int countButtons;
		const unsigned char* axesButtons;

		if (splitscreen)
			axesButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_2, &countButtons);
		else
			axesButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &countButtons);


		//A button
		if (axesButtons[0] == GLFW_PRESS)
		{
			if (!titleScreenButtonPressed)
			{
				titleScreenButtonPressed = true;
				titleScreenCounter = 4.0f;
			}
		}

		//up on directional pad
		if (axesButtons[10] == GLFW_PRESS)
		{
			int type = 0;
			if (splitscreen)
				type = 2;
			ChangeHoldDistance(type, 1);
		}

		//right on the d pad
		if (axesButtons[11] == GLFW_PRESS)
		{
			int type = 0;
			if (splitscreen)
				type = 2;
			UpdateRotVelRayCastObject(type, 5);
		}

		//down on the d pad
		if (axesButtons[12] == GLFW_PRESS)
		{
			int type = 0;
			if (splitscreen)
				type = 2;
			ChangeHoldDistance(type, -1);
		}

		//left on the d pad
		if (axesButtons[13] == GLFW_PRESS)
		{
			int type = 0;
			if (splitscreen)
				type = 2;
			UpdateRotVelRayCastObject(type, -5);
		}


		if (RBPressed == false && axesButtons[5] == GLFW_PRESS)
		{
			RBPressed = true;
			DragOrSpawn(0);
		}
		else if (axesButtons[5] == GLFW_RELEASE)
		{
			RBPressed = false;
			if (draggingFirst)
				g_pSoundManager->PlaySound(4);
			draggingFirst = false;
			rayCastedObject = NULL;

		}

		if (BPressed == false && axesButtons[1] == GLFW_PRESS)
		{
			BPressed = true;
			if (!goingUp)
				goingDown = true;
		}
		else if (axesButtons[1] == GLFW_RELEASE)
		{
			BPressed = false;
		}

		if (XPressed == false && axesButtons[2] == GLFW_PRESS)
		{
			XPressed = true;
			FinishSandwhich(0);
		}
		else if (axesButtons[2] == GLFW_RELEASE)
		{
			XPressed = false;
		}

	}
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	
	if (Input::IsKeyPressed::LeftShift() == true)
	{
		ChangeHoldDistance(1, yoffset);
	}
	else
	{
		UpdateRotVelRayCastObject(1, yoffset*2);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (gameOver)
		return;

	int firstPresent = glfwJoystickPresent(GLFW_JOYSTICK_1);
	int secondPresent = glfwJoystickPresent(GLFW_JOYSTICK_2);

	if (splitscreen && secondPresent != 1)
		UpdateCamera(xpos, ypos, 0);
	else if (firstPresent != 1)
		UpdateCamera(xpos, ypos, 0);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (!titleScreenButtonPressed)
	{
		titleScreenButtonPressed = true;
		titleScreenCounter = 4.0f;
	}
	if (gameOver)
		return;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		DragOrSpawn(0);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		if (draggingFirst)
			g_pSoundManager->PlaySound(4);
		draggingFirst = false;
		rayCastedObject = NULL;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		FinishSandwhich(0);
	}

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);


	// Fullscreen to windowed mode on the PRIMARY monitor (whatever that is)
	if (isAltKeyDown(mods, true) && key == GLFW_KEY_ENTER)
	{
		if (action == GLFW_PRESS)
		{
			::g_IsWindowFullScreen = !::g_IsWindowFullScreen;

			setWindowFullScreenOrWindowed(::g_pGLFWWindow, ::g_IsWindowFullScreen);

		}//if ( action == GLFW_PRESS )
	}//if ( isAltKeyDown(...

	if (gameOver)
		return;

	if (areAllModifierKeysUp(mods))
	{
		switch (key)
		{

		case GLFW_KEY_SPACE: //sending the knife down to cut
		{
			if (action == GLFW_PRESS)
			{
				if (!goingUp)
					goingDown = true;
			}
		}
		break;
		case GLFW_KEY_Y: //switching between splitscreen and not
		{
			if (action == GLFW_PRESS)
			{
				splitscreen = !splitscreen;
			}
		}
		break;

		case GLFW_KEY_K: //switching between splitscreen and not
		{
			if (action == GLFW_PRESS)
			{
				demonstrateCheese = !demonstrateCheese;
			}
		}
		break;

		case GLFW_KEY_C:
		{
			if (action == GLFW_PRESS)
			{
				titleScreen = true;
				titleScreenButtonPressed = false;
				titleScreenCounter = 1;
				
			}
			else if (action == GLFW_RELEASE)
			{
				titleScreen = false;
				titleScreenButtonPressed = true;
				titleScreenCounter = 0;
			}
		}
		break;

		}//switch
	}//if (areAllModifierKeysUp(mods))
	return;
}



// Helper functions
bool isShiftKeyDown(int mods, bool bByItself /*=true*/)
{
	if (bByItself)
	{	// shift by itself?
		if (mods == GLFW_MOD_SHIFT) { return true; }
		else { return false; }
	}
	else
	{	// shift with anything else, too
		if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) { return true; }
		else { return false; }
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool isCtrlKeyDown(int mods, bool bByItself /*=true*/)
{
	if (bByItself)
	{	// shift by itself?
		if (mods == GLFW_MOD_CONTROL) { return true; }
		else { return false; }
	}
	else
	{	// shift with anything else, too
		if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) { return true; }
		else { return false; }
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool isAltKeyDown(int mods, bool bByItself /*=true*/)
{
	if (bByItself)
	{	// shift by itself?
		if (mods == GLFW_MOD_ALT) { return true; }
		else { return false; }
	}
	else
	{	// shift with anything else, too
		if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT) { return true; }
		else { return false; }
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool areAllModifierKeysUp(int mods)
{
	if (isShiftKeyDown(mods)) { return false; }
	if (isCtrlKeyDown(mods)) { return false; }
	if (isAltKeyDown(mods)) { return false; }

	// All of them are up
	return true;
}//areAllModifierKeysUp()

bool areAnyModifierKeysDown(int mods)
{
	if (isShiftKeyDown(mods)) { return true; }
	if (isCtrlKeyDown(mods)) { return true; }
	if (isAltKeyDown(mods)) { return true; }
	// None of them are down
	return false;
}