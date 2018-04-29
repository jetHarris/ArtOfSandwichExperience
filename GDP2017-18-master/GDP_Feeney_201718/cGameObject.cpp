#include "cGameObject.h"

#include "iDebugRenderer.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "cCamera.h"
#include "cMesh.h"
#include "cTriangle.h"
#include "cVAOMeshManager.h"
#include "sVertex_xyz_rgba_n_uv2_bt_4Bones.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\intersect.hpp>
#include "cShaderManager.h"
#include <iostream>
#include <set>
extern cVAOMeshManager* g_pVAOManager;
extern std::vector< cGameObject* >  g_vecGameObjects;
extern cShaderManager* g_pShaderManager;
extern nPhysics::iPhysicsFactory* gPhysicsFactory;
extern nPhysics::iPhysicsWorld* gPhysicsWorld;
extern glm::vec3 pastKnifePos;
extern cGameObject* pCuttingObject;

int newMeshID = 0;


// Start the unique IDs at 1. Why not?
/*static*/ unsigned int cGameObject::m_nextUniqueID = 1;
extern cCamera* g_pTheCamera;

DWORD WINAPI erasePartThreaded(void* pInitialData);

cGameObject::cGameObject()
{
	// Assign a unique ID. 
	// (Be careful of your container calls the default c'tor when copying)
	this->m_UniqueID = cGameObject::m_nextUniqueID++;

	this->pDebugRenderer = NULL;
	this->bDiscardTexture = false;
	this->bIsVisible = false;

	this->spawnerType = 0;

	// Set to NULL (or 0, or nullptr)
	this->pSimpleSkinnedMesh = NULL;		// 0
	this->pAniState = NULL;					// 0
	this->type = ObjectType::MISC;

	rBodyOffset = glm::vec3(0, 0, 0);

	return;
}

cGameObject::~cGameObject()
{
	return;
}


void cGameObject::SetPhysState(cPhysicalProperties &PhysState)
{
	this->m_PhysicalProps = PhysState;
	// cGameObject is friend of cPhysicalProperties, so can set game ID
	this->m_PhysicalProps.m_GameObjectID = this->m_nextUniqueID;
	return;
}

cPhysicalProperties cGameObject::GetPhysState(void)
{
	return this->m_PhysicalProps;
}

void cGameObject::GetPhysState(cPhysicalProperties &PhysState)
{
	PhysState = this->m_PhysicalProps;
	return;
}

glm::vec3 cGameObject::getPosition(void)
{
	return this->m_PhysicalProps.position;
}

void cGameObject::setPosition(glm::vec3 pos)
{
	this->m_PhysicalProps.position = pos;
}

void cGameObject::overwritePotition(glm::vec3 newPosition, bool bOverwiteOldPositionToo /*=true*/)
{
	this->m_PhysicalProps.positionLast = this->m_PhysicalProps.position;
	this->m_PhysicalProps.position = newPosition;
	if (bOverwiteOldPositionToo)
	{	// Clobber the old position, too (so it looks like we're not moving)
		this->m_PhysicalProps.positionLast = this->m_PhysicalProps.position;
	}
	return;
}

glm::quat cGameObject::getQOrientation(void)
{
	return this->m_PhysicalProps.qOrientation;
}

void cGameObject::overwriteQOrientation(glm::quat newOrientation)
{
	this->m_PhysicalProps.qOrientation = newOrientation;
	return;
}

void cGameObject::loadMeshIntoVAONow()
{
	GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

	//attempt 1
	//first attempt with main thread loading stuff into the vao
	g_pVAOManager->loadMeshIntoVAO(loadMeshVector[0], sexyShaderID);
	this->vecMeshes[0].name = this->newMeshName;

	loadMeshVector.clear();
	slicingFinished = false;
	return;
}

void cGameObject::overwriteQOrientationEuler(glm::vec3 newOrientationEuler, bool bIsDegrees /*=true*/)
{
	if (bIsDegrees)
	{
		newOrientationEuler.x = glm::radians(newOrientationEuler.x);
		newOrientationEuler.y = glm::radians(newOrientationEuler.y);
		newOrientationEuler.z = glm::radians(newOrientationEuler.z);
	}
	this->m_PhysicalProps.qOrientation = glm::quat(newOrientationEuler);
	return;
}

void cGameObject::adjQOrientation(glm::quat newOrientation)
{
	this->m_PhysicalProps.qOrientation *= newOrientation;
	return;
}

void cGameObject::adjQOrientationEuler(glm::vec3 newOrientationEuler, bool bIsDegrees /*= true*/)
{
	if (bIsDegrees)
	{
		newOrientationEuler = glm::vec3(glm::radians(newOrientationEuler.x),
			glm::radians(newOrientationEuler.y),
			glm::radians(newOrientationEuler.z));
	}

	// Step 1: make a quaternion that represents the angle we want to rotate
	glm::quat rotationAdjust(newOrientationEuler);
	// Step 2: Multiply this quaternion by the existing quaternion. This "adds" the angle we want.
	this->m_PhysicalProps.qOrientation *= rotationAdjust;
	return;
}

bool cGameObject::RightSideOfPlane(sVertex_xyz_rgba_n_uv2_bt_4Bones&const vertex, glm::vec3&const pointOnPlane, glm::vec3&const testNormal)
{
	//will have to apply object translation, orientation, and scale
	glm::vec4 tempPos4 = glm::vec4(vertex.x, vertex.y, vertex.z, 1);
	//tempPos4 = tempPos4 * glm::toMat4(this->qOrientation);
	tempPos4 = tempPos4 * this->worldMatrix;
	//tempPos 

	glm::vec3 tempPos = glm::vec3(tempPos4.x, tempPos4.y, tempPos4.z);

	glm::vec3 direction(tempPos - pointOnPlane);

	double dprod = glm::dot(testNormal, direction);

	if (dprod <= 0)
	{
		return true;
	}
	return false;
}

void cGameObject::CleanOldVertices(std::vector<sVertex_xyz_rgba_n_uv2_bt_4Bones>* vertices, std::vector<cTriangle>* triangles)
{
	//go through all the triangles making some sort of list of ids of vertices that are still in the model
	std::set<int> goodVertexIds;
	int trianglesSize = triangles->size();
	for (int i = 0; i < trianglesSize; ++i)
	{
		goodVertexIds.insert((*triangles)[i].vertex_ID_0);
		goodVertexIds.insert((*triangles)[i].vertex_ID_1);
		goodVertexIds.insert((*triangles)[i].vertex_ID_2);
	}

	std::vector<sVertex_xyz_rgba_n_uv2_bt_4Bones>* newVertices = new std::vector<sVertex_xyz_rgba_n_uv2_bt_4Bones>();
	std::map<int, int> deletedOffsets;
	int offset = 0;
	int verticesSize = vertices->size();
	//go through the vertices vector making a new vector of only good vertices
	for (int i = 0; i < verticesSize; ++i)
	{

		//if the current vertex is a vertex still displayed in the model
		if (goodVertexIds.find(i) != goodVertexIds.end())
		{
			newVertices->push_back((*vertices)[i]);
		}
		else
		{
			offset++;
		}

		deletedOffsets[i] = offset;

	}

	std::cout << "Old Size = " << vertices->size() << std::endl;
	//update the vertices vector
	(*vertices) = (*newVertices);
	std::cout << "New Size = " << newVertices->size() << std::endl;
	//go through all the triangles properly setting the new vertex ids
	for (int i = 0; i < trianglesSize; ++i)
	{
		(*triangles)[i].vertex_ID_0 = (*triangles)[i].vertex_ID_0 - deletedOffsets[(*triangles)[i].vertex_ID_0];
		(*triangles)[i].vertex_ID_1 = (*triangles)[i].vertex_ID_1 - deletedOffsets[(*triangles)[i].vertex_ID_1];
		(*triangles)[i].vertex_ID_2 = (*triangles)[i].vertex_ID_2 - deletedOffsets[(*triangles)[i].vertex_ID_2];
	}
}

void cGameObject::ReorientPositionAndVertices(std::vector<sVertex_xyz_rgba_n_uv2_bt_4Bones>* vertices, std::vector<cTriangle>* triangles, bool makePhysics)
{
	glm::vec3 holderPosition = this->getPosition();
	glm::vec3 maxXYZ = glm::vec3(-9999, -9999, -9999);
	glm::vec3 minXYZ = glm::vec3(9999, 9999, 9999);


	std::map<int, int> consideredVertices;

	//instead of going through all of the vertices, should go through all the triangles
	//as that is now a better representation of what the model currently is

	//now go through all the points applying the model matrix
	//calculate a max xyz and min xyz
	int trianglesSize = triangles->size();
	for (int i = 0; i < trianglesSize; ++i)
	{

		if (consideredVertices[(*triangles)[i].vertex_ID_0] != 1)
		{
			glm::vec4 tempPos4In = glm::vec4((*vertices)[(*triangles)[i].vertex_ID_0].x, (*vertices)[(*triangles)[i].vertex_ID_0].y, (*vertices)[(*triangles)[i].vertex_ID_0].z, 1);
			tempPos4In = tempPos4In * this->worldMatrix;

			if (tempPos4In.x < minXYZ.x)
				minXYZ.x = tempPos4In.x;
			if (tempPos4In.y < minXYZ.y)
				minXYZ.y = tempPos4In.y;
			if (tempPos4In.z < minXYZ.z)
				minXYZ.z = tempPos4In.z;

			if (tempPos4In.x > maxXYZ.x)
				maxXYZ.x = tempPos4In.x;
			if (tempPos4In.y > maxXYZ.y)
				maxXYZ.y = tempPos4In.y;
			if (tempPos4In.z > maxXYZ.z)
				maxXYZ.z = tempPos4In.z;

			consideredVertices[(*triangles)[i].vertex_ID_0] = 1;
		}

		if (consideredVertices[(*triangles)[i].vertex_ID_1] != 1)
		{
			glm::vec4 tempPos4In = glm::vec4((*vertices)[(*triangles)[i].vertex_ID_1].x, (*vertices)[(*triangles)[i].vertex_ID_1].y, (*vertices)[(*triangles)[i].vertex_ID_1].z, 1);
			tempPos4In = tempPos4In * this->worldMatrix;

			if (tempPos4In.x < minXYZ.x)
				minXYZ.x = tempPos4In.x;
			if (tempPos4In.y < minXYZ.y)
				minXYZ.y = tempPos4In.y;
			if (tempPos4In.z < minXYZ.z)
				minXYZ.z = tempPos4In.z;

			if (tempPos4In.x > maxXYZ.x)
				maxXYZ.x = tempPos4In.x;
			if (tempPos4In.y > maxXYZ.y)
				maxXYZ.y = tempPos4In.y;
			if (tempPos4In.z > maxXYZ.z)
				maxXYZ.z = tempPos4In.z;

			consideredVertices[(*triangles)[i].vertex_ID_1] = 1;
		}

		if (consideredVertices[(*triangles)[i].vertex_ID_2] != 1)
		{
			glm::vec4 tempPos4In = glm::vec4((*vertices)[(*triangles)[i].vertex_ID_2].x, (*vertices)[(*triangles)[i].vertex_ID_2].y, (*vertices)[(*triangles)[i].vertex_ID_2].z, 1);
			tempPos4In = tempPos4In * this->worldMatrix;

			if (tempPos4In.x < minXYZ.x)
				minXYZ.x = tempPos4In.x;
			if (tempPos4In.y < minXYZ.y)
				minXYZ.y = tempPos4In.y;
			if (tempPos4In.z < minXYZ.z)
				minXYZ.z = tempPos4In.z;

			if (tempPos4In.x > maxXYZ.x)
				maxXYZ.x = tempPos4In.x;
			if (tempPos4In.y > maxXYZ.y)
				maxXYZ.y = tempPos4In.y;
			if (tempPos4In.z > maxXYZ.z)
				maxXYZ.z = tempPos4In.z;

			consideredVertices[(*triangles)[i].vertex_ID_2] = 1;
		}

	}

	//use that to calculate an average point
	glm::vec3 newPosition = glm::vec3(0);
	newPosition.x = (maxXYZ.x + minXYZ.x) / 2.0f;
	newPosition.y = (maxXYZ.y + minXYZ.y) / 2.0f;
	//newPosition.z = (maxXYZ.z + minXYZ.z)/ 2.0f;

	//dunno why this next line is needed, it shouldnt be
	newPosition.x += this->getPosition().x;
	newPosition.y += this->getPosition().y;
	//newPosition.z -= this->getPosition().z;
	//this is a hack for z position it sets it to the slice location, but the center of the object is what we need
	//newPosition.z += pastKnifePos.z;
	newPosition.z = pastKnifePos.z + (abs(maxXYZ.z - minXYZ.z) / 2.0f);


	//note this might have to be turned around
	glm::vec3 difference = newPosition - this->getPosition();

	//use if the difference between the average point and the current location to
	//modify all of the vertices //apply model matrix, translate, apply reverse of model matrix (not sure if reverse of new or old)
	int verticesSize = vertices->size();
	//std::vector<glm::vec3> convexHullVertices;
	for (int i = 0; i < verticesSize; ++i)
	{
		glm::vec4 tempPos4In = glm::vec4((*vertices)[i].x, (*vertices)[i].y, (*vertices)[i].z, 1);
		tempPos4In = tempPos4In * this->worldMatrix;
		glm::vec3 tempPosIn = glm::vec3(tempPos4In.x, tempPos4In.y, tempPos4In.z);

		tempPosIn = tempPosIn + difference;
		tempPos4In = glm::vec4(tempPosIn.x, tempPosIn.y, tempPosIn.z, 1);

		////here is where we will add to a float array/vector for the convex hull
		//if (consideredVertices[i] == 1)
		//{
		//	convexHullVertices.push_back(tempPosIn);
		//}

		//note the inverse world matrix may have to be updated
		tempPos4In = tempPos4In * this->inverseWorldMatrix;
		(*vertices)[i].x = tempPos4In.x;
		(*vertices)[i].y = tempPos4In.y;
		(*vertices)[i].z = tempPos4In.z;



	}

	this->setPosition(newPosition);
	//make a new physics box based on the max and min values
	if (makePhysics)
	{
		nPhysics::iShape* shape;
		nPhysics::sRigidBodyDesc desc;
		desc.Mass = 1.0f;
		desc.Position = this->getPosition();

		//if (this->type == ObjectType::TOMATO || this->type == ObjectType::SALAMI)
		//{
		//	float xWidth = glm::distance(maxXYZ.x, minXYZ.x) / 2.0f;
		//	float yWidth = glm::distance(maxXYZ.y, minXYZ.y) / 2.0f;
		//	float zWidth = glm::distance(maxXYZ.z, minXYZ.z) / 2.0f;
		//	//shape = gPhysicsFactory->CreateCylinder(glm::vec3((maxXYZ.x - minXYZ.x) / 2.0f, (maxXYZ.y - minXYZ.y) / 2.0f, (maxXYZ.z - minXYZ.z) / 2.0f));
		//	shape = gPhysicsFactory->CreateCylinder(glm::vec3(xWidth, yWidth, zWidth));
		//	std::cout << "X: " << xWidth << " Y: " << yWidth << " Z: " << zWidth << std::endl;
		//}
		//else
		shape = gPhysicsFactory->CreateBox(glm::vec3((maxXYZ.x - minXYZ.x) / 2.0f, (maxXYZ.y - minXYZ.y) / 2.0f, (maxXYZ.z - minXYZ.z) / 2.0f));


		//std::cout << "X: " << this->getPosition().x
		//shape = gPhysicsFactory->CreateConvexHull();
		//int hullVerticesSize = convexHullVertices.size();
		//for (int i = 0; i < hullVerticesSize; i++)
		//{
		//	shape->AddPoint(convexHullVertices[i]);
		//}


		this->physics = true;
		this->cuttable = false;
		this->rBody = gPhysicsFactory->CreateRigidBody(desc, shape);
		::gPhysicsWorld->AddRigidBody(this->rBody);
		this->rBodyOrientationOffset = this->getQOrientation();

	}

}

void SphereEdge(glm::vec3 & pointInSphere, glm::vec3& center, float& radius)
{
	pointInSphere = (glm::normalize(pointInSphere - center))* (radius + 0.1f) + center;
}

glm::vec3 cGameObject::FindIntersectionPoint(sVertex_xyz_rgba_n_uv2_bt_4Bones&const vertexIn, sVertex_xyz_rgba_n_uv2_bt_4Bones&const vertexOut, glm::vec3&const pointOnPlane, glm::vec3&const testNormal)
{
	glm::vec4 tempPos4In = glm::vec4(vertexIn.x, vertexIn.y, vertexIn.z, 1);
	tempPos4In = tempPos4In * this->worldMatrix;
	glm::vec3 tempPosIn = glm::vec3(tempPos4In.x, tempPos4In.y, tempPos4In.z);

	glm::vec4 tempPos4Out = glm::vec4(vertexOut.x, vertexOut.y, vertexOut.z, 1);
	tempPos4Out = tempPos4Out * this->worldMatrix;
	glm::vec3 tempPosOut = glm::vec3(tempPos4Out.x, tempPos4Out.y, tempPos4Out.z);

	glm::vec3 direction = glm::normalize(tempPosOut - tempPosIn);
	float dist;
	glm::intersectRayPlane(tempPosIn, direction, pointOnPlane, testNormal, dist);


	glm::vec3 returnVector = (tempPosIn + (direction * dist));
	glm::vec4 tempReturnVector = glm::vec4();
	tempReturnVector.x = returnVector.x;
	tempReturnVector.y = returnVector.y;
	tempReturnVector.z = returnVector.z;
	tempReturnVector.w = 1;

	tempReturnVector = tempReturnVector * this->inverseWorldMatrix;

	returnVector.x = tempReturnVector.x;
	returnVector.y = tempReturnVector.y;
	returnVector.z = tempReturnVector.z;

	return returnVector;
}

glm::vec3 cGameObject::AveragePoint(sVertex_xyz_rgba_n_uv2_bt_4Bones&const vertex1, sVertex_xyz_rgba_n_uv2_bt_4Bones&const vertex2)
{
	glm::vec4 tempPos4In = glm::vec4(vertex1.x, vertex1.y, vertex1.z, 1);
	tempPos4In = tempPos4In * this->worldMatrix;
	glm::vec3 tempPosIn = glm::vec3(tempPos4In.x, tempPos4In.y, tempPos4In.z);

	glm::vec4 tempPos4Out = glm::vec4(vertex2.x, vertex2.y, vertex2.z, 1);
	tempPos4Out = tempPos4Out * this->worldMatrix;
	glm::vec3 tempPosOut = glm::vec3(tempPos4Out.x, tempPos4Out.y, tempPos4Out.z);


	glm::vec3 returnVector = tempPosIn + tempPosOut;
	returnVector *= 0.5f;
	return returnVector;
}

void cGameObject::calcXYZDepths(float & xIn, float & yIn, float & zIn)
{
	glm::vec3 maxXYZ = glm::vec3(-9999, -9999, -9999);
	glm::vec3 minXYZ = glm::vec3(9999, 9999, 9999);
	cMesh* theOldMesh = g_pVAOManager->lookupMeshFromName(this->vecMeshes[0].name);

	int verticesSize = theOldMesh->numberOfVertices;

	for (int i = 0; i < verticesSize; ++i)
	{
		glm::vec4 tempPos4In = glm::vec4(theOldMesh->pVertices[i].x, theOldMesh->pVertices[i].y, theOldMesh->pVertices[i].z, 1);
		tempPos4In = tempPos4In * this->worldMatrix;

		if (tempPos4In.x < minXYZ.x)
			minXYZ.x = tempPos4In.x;
		if (tempPos4In.y < minXYZ.y)
			minXYZ.y = tempPos4In.y;
		if (tempPos4In.z < minXYZ.z)
			minXYZ.z = tempPos4In.z;

		if (tempPos4In.x > maxXYZ.x)
			maxXYZ.x = tempPos4In.x;
		if (tempPos4In.y > maxXYZ.y)
			maxXYZ.y = tempPos4In.y;
		if (tempPos4In.z > maxXYZ.z)
			maxXYZ.z = tempPos4In.z;
	}

	xIn = (abs(maxXYZ.x - minXYZ.x)) / 2.0f;
	yIn = (abs(maxXYZ.y - minXYZ.y)) / 2.0f;
	zIn = (abs(maxXYZ.z - minXYZ.z)) / 2.0f;

}

void CopyVertexValues(sVertex_xyz_rgba_n_uv2_bt_4Bones* first, sVertex_xyz_rgba_n_uv2_bt_4Bones* second)
{
	first->a = second->a;
	first->b = second->b;
	first->bx = second->bx;
	first->by = second->by;
	first->bz = second->bz;
	first->g = second->g;
	first->nx = second->nx;
	first->ny = second->ny;
	first->nz = second->nz;
	first->r = second->r;
	first->tx = second->tx;
	first->ty = second->ty;
	first->tz = second->tz;
	first->u1 = second->u1;
	first->u2 = second->u2;
	first->v1 = second->v1;
	first->v2 = second->v2;
	first->x = second->x;
	first->y = second->y;
	first->z = second->z;
}

void AverageVertex(sVertex_xyz_rgba_n_uv2_bt_4Bones* first, sVertex_xyz_rgba_n_uv2_bt_4Bones* second, sVertex_xyz_rgba_n_uv2_bt_4Bones* third)
{
	first->a = second->a;
	first->b = second->b;
	first->bx = (second->bx + third->bx) / 2.0f;
	first->by = (second->by + third->by) / 2.0f;
	first->bz = (second->bz + third->bz) / 2.0f;
	first->g = second->g;
	first->nx = (second->nx + third->nx) / 2.0f;
	first->ny = (second->ny + third->ny) / 2.0f;
	first->nz = (second->nz + third->nz) / 2.0f;
	first->r = second->r;
	first->tx = second->tx;
	first->ty = second->ty;
	first->tz = second->tz;
	first->u1 = (second->u1 + third->u1) / 2.0f;
	first->u2 = (second->u2 + third->u2) / 2.0f;
	first->v1 = (second->v1 + third->v1) / 2.0f;
	first->v2 = (second->v2 + third->v2) / 2.0f;
	first->x = (second->x + third->x) / 2.0f;
	first->y = (second->y + third->y) / 2.0f;
	first->z = (second->z + third->z) / 2.0f;
}

DWORD WINAPI SliceThreaded(void * pInitialData)
{
	cGameObject* theObject = (cGameObject*)(pInitialData);

	theObject->testNormal = glm::normalize(theObject->testNormal);
	theObject->RecalculateWorldMatrix();

	cMesh* theOldMesh = g_pVAOManager->lookupMeshFromName(theObject->vecMeshes[0].name);
	cMesh theNewMesh;
	//basically make a new mesh using the old mesh
	theNewMesh.maxExtent = theOldMesh->maxExtent;
	theNewMesh.maxExtentXYZ = theOldMesh->maxExtentXYZ;
	theNewMesh.maxXYZ = theOldMesh->maxXYZ;
	theNewMesh.minXYZ = theOldMesh->minXYZ;
	theNewMesh.name = theOldMesh->name;
	theNewMesh.scaleForUnitBBox = theOldMesh->scaleForUnitBBox;

	theObject->vecMeshes[0].bDisableBackFaceCulling = true;

	std::vector<sVertex_xyz_rgba_n_uv2_bt_4Bones>* vertices = new std::vector<sVertex_xyz_rgba_n_uv2_bt_4Bones>();
	bool meshInCut = false;

	for (int i = 0; i < theOldMesh->numberOfVertices; ++i)
	{
		vertices->push_back(theOldMesh->pVertices[i]);
	}



	std::vector<cTriangle>* triangles = new std::vector<cTriangle>();
	std::vector<std::pair<int, int>> pairs;
	int outSideCount = 0;
	bool firstInside = false;
	bool secondInside = false;
	bool thirdInside = false;

	for (int i = 0; i < theOldMesh->numberOfTriangles; ++i)
	{
		outSideCount = 0;
		firstInside = true;
		secondInside = true;
		thirdInside = true;

		if (theObject->RightSideOfPlane(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theObject->pointOnPlane, theObject->testNormal))
		{
			outSideCount++;
			firstInside = false;
		}
		if (theObject->RightSideOfPlane(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theObject->pointOnPlane, theObject->testNormal))
		{
			outSideCount++;
			secondInside = false;
		}
		if (theObject->RightSideOfPlane(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theObject->pointOnPlane, theObject->testNormal))
		{
			outSideCount++;
			thirdInside = false;
		}

		if (outSideCount == 0)
			triangles->push_back(theOldMesh->pTriangles[i]);
		else if (outSideCount == 1)
		{
			meshInCut = true;

			sVertex_xyz_rgba_n_uv2_bt_4Bones* firstVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();
			sVertex_xyz_rgba_n_uv2_bt_4Bones* secondVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();
			sVertex_xyz_rgba_n_uv2_bt_4Bones* thirdVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();

			glm::vec3 intersection1 = glm::vec3(0);
			glm::vec3 intersection2 = glm::vec3(0);

			glm::vec3 middlePoint = glm::vec3(0);


			if (!firstInside)
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theObject->pointOnPlane, theObject->testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theObject->pointOnPlane, theObject->testNormal);

				middlePoint = theObject->AveragePoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0]);
				AverageVertex(thirdVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
			}
			else if (!secondInside)
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theObject->pointOnPlane, theObject->testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theObject->pointOnPlane, theObject->testNormal);


				middlePoint = theObject->AveragePoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
				AverageVertex(thirdVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
			}
			else
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theObject->pointOnPlane, theObject->testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theObject->pointOnPlane, theObject->testNormal);

				middlePoint = theObject->AveragePoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				AverageVertex(thirdVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
			}

			firstVert->x = intersection1.x;
			firstVert->y = intersection1.y;
			firstVert->z = intersection1.z;

			secondVert->x = intersection2.x;
			secondVert->y = intersection2.y;
			secondVert->z = intersection2.z;

			//and to the vertices vector
			vertices->push_back(*firstVert);
			vertices->push_back(*secondVert);

			int size = vertices->size();
			//add those points to the pairs
			pairs.push_back(std::make_pair<int, int>(size - 1, size - 2));

			vertices->push_back(*thirdVert);
			size = vertices->size();

			//create and add the three new triangles
			cTriangle* firstTri = new cTriangle();
			cTriangle* secondTri = new cTriangle();
			cTriangle* thirdTri = new cTriangle();

			if (!firstInside)
			{
				firstTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_1;
				secondTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_2;
			}
			else if (!secondInside)
			{
				firstTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_0;
				secondTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_2;
			}
			else
			{
				firstTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_0;
				secondTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_1;
			}


			firstTri->vertex_ID_1 = size - 3;
			firstTri->vertex_ID_2 = size - 1;

			secondTri->vertex_ID_1 = size - 2;
			secondTri->vertex_ID_2 = size - 1;


			thirdTri->vertex_ID_0 = size - 1;
			thirdTri->vertex_ID_1 = size - 2;
			thirdTri->vertex_ID_2 = size - 3;



			triangles->push_back(*firstTri);
			triangles->push_back(*secondTri);
			triangles->push_back(*thirdTri);


		}
		else if (outSideCount == 2)
		{
			meshInCut = true;
			//use the vertex inside Id,
			glm::vec3 intersection1 = glm::vec3(0);
			glm::vec3 intersection2 = glm::vec3(0);
			sVertex_xyz_rgba_n_uv2_bt_4Bones* firstVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();
			sVertex_xyz_rgba_n_uv2_bt_4Bones* secondVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();

			//the triangle we will be constructing
			cTriangle* newTri = new cTriangle();

			bool alreadyExistingIntersection = false;


			//find the two intersection points
			if (firstInside)
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theObject->pointOnPlane, theObject->testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theObject->pointOnPlane, theObject->testNormal);

				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				newTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_0;
			}
			else if (secondInside)
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theObject->pointOnPlane, theObject->testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theObject->pointOnPlane, theObject->testNormal);

				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				newTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_1;
			}
			else
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theObject->pointOnPlane, theObject->testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theObject->pointOnPlane, theObject->testNormal);

				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
				newTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_2;
			}

			//set firstVert to intersection one location
			firstVert->x = intersection1.x;
			firstVert->y = intersection1.y;
			firstVert->z = intersection1.z;

			secondVert->x = intersection2.x;
			secondVert->y = intersection2.y;
			secondVert->z = intersection2.z;


			//and to the vertices vector
			vertices->push_back(*firstVert);
			vertices->push_back(*secondVert);
			int size = vertices->size();
			//add those points to the pairs
			pairs.push_back(std::make_pair<int, int>(size - 1, size - 2));

			//make a new triangle using the vertex inside and the two insersection points
			newTri->vertex_ID_1 = pairs.back().first;
			newTri->vertex_ID_2 = pairs.back().second;

			triangles->push_back(*newTri);

		}
	}

	if (meshInCut)
	{
		//make a final vertex
		sVertex_xyz_rgba_n_uv2_bt_4Bones* finalVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();
		CopyVertexValues(finalVert, &(*vertices)[pairs.front().first]);

		std::vector<std::pair<int, int>> pairsSliceFace;
		//go through pairs adding all the vertices to vertices again, and add the new ids to pairsSliceFace
		//also update all their normals to be the same as the slice face
		//then used pairsSliceFace instead of pairs
		int pairsSize = pairs.size();
		int vertSize = vertices->size();
		for (int i = 0; i < pairsSize; ++i)
		{
			vertices->push_back((*vertices)[pairs[i].first]);
			sVertex_xyz_rgba_n_uv2_bt_4Bones* first = &vertices->back();
			first->nx = theObject->testNormal.x;
			first->ny = theObject->testNormal.y;
			first->nz = theObject->testNormal.z;

			vertices->push_back((*vertices)[pairs[i].second]);
			sVertex_xyz_rgba_n_uv2_bt_4Bones* second = &vertices->back();
			second->nx = theObject->testNormal.x;
			second->ny = theObject->testNormal.y;
			second->nz = theObject->testNormal.z;

			vertSize += 2;
			pairsSliceFace.push_back(std::make_pair<int, int>(vertSize - 1, vertSize - 2));
		}


		//average it based on all the pairs 
		pairsSize = pairsSliceFace.size();
		vertSize = vertices->size();
		glm::vec3 averagePos = glm::vec3(0);
		float divisionValue = pairsSize * 2;
		for (int i = 0; i < pairsSize; ++i)
		{
			cTriangle* newTri = new cTriangle();
			newTri->vertex_ID_0 = pairsSliceFace[i].first;
			newTri->vertex_ID_1 = pairsSliceFace[i].second;
			newTri->vertex_ID_2 = vertSize;

			averagePos.x += (*vertices)[pairsSliceFace[i].first].x;
			averagePos.y += (*vertices)[pairsSliceFace[i].first].y;
			averagePos.z += (*vertices)[pairsSliceFace[i].first].z;

			averagePos.x += (*vertices)[pairsSliceFace[i].second].x;
			averagePos.y += (*vertices)[pairsSliceFace[i].second].y;
			averagePos.z += (*vertices)[pairsSliceFace[i].second].z;

			triangles->push_back(*newTri);
		}

		averagePos = averagePos / divisionValue;
		finalVert->x = averagePos.x;
		finalVert->y = averagePos.y;
		finalVert->z = averagePos.z;
		finalVert->nx = theObject->testNormal.x;
		finalVert->ny = theObject->testNormal.y;
		finalVert->nz = theObject->testNormal.z;

		vertices->push_back(*finalVert);

		//before reassigning the vertices make a copy of the object and call this method on it, but reverse the normal
		if (theObject->makeNewMesh)
		{
			cGameObject* theOtherHalf = new cGameObject();
			//somehow the mesh is getting more and more complex, to combat this we will test for type here and simply make
			switch (theObject->type)
			{
			case ObjectType::CHEESE:
			{
				sMeshDrawInfo meshInfo;
				meshInfo.name = "cheese.ply";
				meshInfo.scale = 2.0f;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("CheeseTexture.bmp", 1.0f));
				theOtherHalf->vecMeshes.push_back(meshInfo);
			}
			break;

			case ObjectType::SALAMI:
			{
				sMeshDrawInfo meshInfo;
				meshInfo.name = "meat.ply";
				meshInfo.scale = 2.0f;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("MeatTexture.bmp", 1.0f));
				theOtherHalf->vecMeshes.push_back(meshInfo);
			}
			break;

			case ObjectType::TOMATO:
			{
				sMeshDrawInfo meshInfo;
				meshInfo.name = "tomato.ply";
				meshInfo.scale = 2.0f;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("tomatoTexture.bmp", 1.0f));
				theOtherHalf->vecMeshes.push_back(meshInfo);
			}
			break;

			case ObjectType::BREADSLICE:
			{
				sMeshDrawInfo meshInfo;
				meshInfo.name = "breadLoaf.ply";
				meshInfo.scale = 2.0f;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("BreadLoafT.bmp", 1.0f));
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("breadInside.bmp", 0.0f));
				theOtherHalf->vecMeshes.push_back(meshInfo);
			}
			break;
			}
			//a new version of the original mesh

			theOtherHalf->type = theObject->type;
			pCuttingObject = theOtherHalf;
			theOtherHalf->setPosition(theObject->getPosition());
			theOtherHalf->worldMatrix = theObject->worldMatrix;
			theOtherHalf->m_PhysicalProps.qOrientation = theObject->m_PhysicalProps.qOrientation;
			::g_vecGameObjects.push_back(theOtherHalf);

			::g_vecGameObjects.back()->Slice(false, theObject->pointOnPlane, theObject->testNormal * -1.0f);
			//::g_vecGameObjects.back()->setPosition(::g_vecGameObjects.back()->getPosition() + (testNormal * 0.5f));
			::g_vecGameObjects.back()->vecMeshes[0].bDisableBackFaceCulling = true;
			::g_vecGameObjects.back()->cuttable = true;
		}


		if (theObject->makeNewMesh)
		{
			theNewMesh.name = theOldMesh->name + std::to_string(newMeshID);
			theObject->newMeshName = theObject->vecMeshes[0].name + std::to_string(newMeshID);
			newMeshID++;
			theObject->ReorientPositionAndVertices(vertices, triangles, true);
		}
		else
		{
			theNewMesh.name = theOldMesh->name + std::to_string(newMeshID);
			theObject->newMeshName = theObject->vecMeshes[0].name + std::to_string(newMeshID);
			newMeshID++;
		}



		theNewMesh.pVertices = &(*vertices)[0];
		theNewMesh.numberOfVertices = vertices->size();
		theNewMesh.numberOfTriangles = triangles->size();
		theNewMesh.pTriangles = &(*triangles)[0];

		theObject->loadMeshVector.push_back(theNewMesh);
		theObject->slicingFinished = true;

		//GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");
		//g_pVAOManager->loadMeshIntoVAO(theNewMesh, sexyShaderID);

	}

	return meshInCut;
	//experiment to try and fix a broken thing
	//theOldMesh = theNewMesh;

}


bool SliceP(bool makeNewMesh, glm::vec3 pointOnPlane, glm::vec3 testNormal, cGameObject* theObject)
{
	//the following block is if you want this method to be threaded
	//there is a bug with the shadow disappearing for a like 1/16th of a second

	//////theObject->makeNewMesh = makeNewMesh;
	//////theObject->pointOnPlane = pointOnPlane;
	//////theObject->testNormal = testNormal;

	//////cGameObject *thisP = theObject;

	////////now start up the thread that does the stuff

	//////theObject->slicingHandle = CreateThread(	//_beginthreadex()
	//////	NULL, // Attributes
	//////	0,		// 0 = default stack size,
	//////	SliceThreaded,
	//////	(void*)thisP,		// Parameters to pass to the thread function
	//////	0,			// 0 or CREATE_SUSPENDED, ResumeThread()
	//////	&theObject->slicingAddress);

	//////return true;


	//this is the start of the not threaded version
	testNormal = glm::normalize(testNormal);
	theObject->RecalculateWorldMatrix();

	cMesh* theOldMesh = g_pVAOManager->lookupMeshFromName(theObject->vecMeshes[0].name);
	cMesh theNewMesh;
	//basically make a new mesh using the old mesh
	theNewMesh.maxExtent = theOldMesh->maxExtent;
	theNewMesh.maxExtentXYZ = theOldMesh->maxExtentXYZ;
	theNewMesh.maxXYZ = theOldMesh->maxXYZ;
	theNewMesh.minXYZ = theOldMesh->minXYZ;
	theNewMesh.name = theOldMesh->name;
	theNewMesh.scaleForUnitBBox = theOldMesh->scaleForUnitBBox;

	theObject->vecMeshes[0].bDisableBackFaceCulling = true;

	std::vector<sVertex_xyz_rgba_n_uv2_bt_4Bones>* vertices = new std::vector<sVertex_xyz_rgba_n_uv2_bt_4Bones>();
	bool meshInCut = false;

	for (int i = 0; i < theOldMesh->numberOfVertices; ++i)
	{
		vertices->push_back(theOldMesh->pVertices[i]);
	}



	std::vector<cTriangle>* triangles = new std::vector<cTriangle>();
	std::vector<std::pair<int, int>> pairs;
	int outSideCount = 0;
	bool firstInside = false;
	bool secondInside = false;
	bool thirdInside = false;

	//loop for finding intersection points with the plane
	for (int i = 0; i < theOldMesh->numberOfTriangles; ++i)
	{
		outSideCount = 0;
		firstInside = true;
		secondInside = true;
		thirdInside = true;

		if (theObject->RightSideOfPlane(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], pointOnPlane, testNormal))
		{
			outSideCount++;
			firstInside = false;
		}
		if (theObject->RightSideOfPlane(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], pointOnPlane, testNormal))
		{
			outSideCount++;
			secondInside = false;
		}
		if (theObject->RightSideOfPlane(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], pointOnPlane, testNormal))
		{
			outSideCount++;
			thirdInside = false;
		}

		if (outSideCount == 0)
			triangles->push_back(theOldMesh->pTriangles[i]);
		else if (outSideCount == 1)
		{
			meshInCut = true;

			sVertex_xyz_rgba_n_uv2_bt_4Bones* firstVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();
			sVertex_xyz_rgba_n_uv2_bt_4Bones* secondVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();
			sVertex_xyz_rgba_n_uv2_bt_4Bones* thirdVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();

			glm::vec3 intersection1 = glm::vec3(0);
			glm::vec3 intersection2 = glm::vec3(0);

			glm::vec3 middlePoint = glm::vec3(0);


			if (!firstInside)
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], pointOnPlane, testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], pointOnPlane, testNormal);

				middlePoint = theObject->AveragePoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0]);
				AverageVertex(thirdVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
			}
			else if (!secondInside)
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], pointOnPlane, testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], pointOnPlane, testNormal);


				middlePoint = theObject->AveragePoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
				AverageVertex(thirdVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
			}
			else
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], pointOnPlane, testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], pointOnPlane, testNormal);

				middlePoint = theObject->AveragePoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				AverageVertex(thirdVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
			}

			firstVert->x = intersection1.x;
			firstVert->y = intersection1.y;
			firstVert->z = intersection1.z;

			secondVert->x = intersection2.x;
			secondVert->y = intersection2.y;
			secondVert->z = intersection2.z;

			//and to the vertices vector
			vertices->push_back(*firstVert);
			vertices->push_back(*secondVert);

			int size = vertices->size();
			//add those points to the pairs
			pairs.push_back(std::make_pair<int, int>(size - 1, size - 2));

			vertices->push_back(*thirdVert);
			size = vertices->size();

			//create and add the three new triangles
			cTriangle* firstTri = new cTriangle();
			cTriangle* secondTri = new cTriangle();
			cTriangle* thirdTri = new cTriangle();

			if (!firstInside)
			{
				firstTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_1;
				secondTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_2;
			}
			else if (!secondInside)
			{
				firstTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_0;
				secondTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_2;
			}
			else
			{
				firstTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_0;
				secondTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_1;
			}


			firstTri->vertex_ID_1 = size - 3;
			firstTri->vertex_ID_2 = size - 1;

			secondTri->vertex_ID_1 = size - 2;
			secondTri->vertex_ID_2 = size - 1;


			thirdTri->vertex_ID_0 = size - 1;
			thirdTri->vertex_ID_1 = size - 2;
			thirdTri->vertex_ID_2 = size - 3;



			triangles->push_back(*firstTri);
			triangles->push_back(*secondTri);
			triangles->push_back(*thirdTri);


		}
		else if (outSideCount == 2)
		{
			meshInCut = true;
			//use the vertex inside Id,
			glm::vec3 intersection1 = glm::vec3(0);
			glm::vec3 intersection2 = glm::vec3(0);
			sVertex_xyz_rgba_n_uv2_bt_4Bones* firstVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();
			sVertex_xyz_rgba_n_uv2_bt_4Bones* secondVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();

			//the triangle we will be constructing
			cTriangle* newTri = new cTriangle();

			bool alreadyExistingIntersection = false;


			//find the two intersection points
			if (firstInside)
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], pointOnPlane, testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], pointOnPlane, testNormal);

				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				newTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_0;
			}
			else if (secondInside)
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], pointOnPlane, testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], pointOnPlane, testNormal);

				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2]);
				newTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_1;
			}
			else
			{

				intersection1 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1], pointOnPlane, testNormal);

				intersection2 = theObject->FindIntersectionPoint(theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_2], theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0], pointOnPlane, testNormal);

				CopyVertexValues(firstVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_0]);
				CopyVertexValues(secondVert, &theOldMesh->pVertices[theOldMesh->pTriangles[i].vertex_ID_1]);
				newTri->vertex_ID_0 = theOldMesh->pTriangles[i].vertex_ID_2;
			}

			//set firstVert to intersection one location
			firstVert->x = intersection1.x;
			firstVert->y = intersection1.y;
			firstVert->z = intersection1.z;

			secondVert->x = intersection2.x;
			secondVert->y = intersection2.y;
			secondVert->z = intersection2.z;

			//and to the vertices vector
			vertices->push_back(*firstVert);
			vertices->push_back(*secondVert);
			int size = vertices->size();
			//add those points to the pairs
			pairs.push_back(std::make_pair<int, int>(size - 1, size - 2));

			//make a new triangle using the vertex inside and the two insersection points
			newTri->vertex_ID_1 = pairs.back().first;
			newTri->vertex_ID_2 = pairs.back().second;

			triangles->push_back(*newTri);

		}
	}

	if (meshInCut)
	{
		//make a final vertex
		sVertex_xyz_rgba_n_uv2_bt_4Bones* finalVert = new sVertex_xyz_rgba_n_uv2_bt_4Bones();
		CopyVertexValues(finalVert, &(*vertices)[pairs.front().first]);

		std::vector<std::pair<int, int>> pairsSliceFace;
		//go through pairs adding all the vertices to vertices again, and add the new ids to pairsSliceFace
		//also update all their normals to be the same as the slice face
		//then used pairsSliceFace instead of pairs
		int pairsSize = pairs.size();
		int vertSize = vertices->size();
		for (int i = 0; i < pairsSize; ++i)
		{
			vertices->push_back((*vertices)[pairs[i].first]);
			sVertex_xyz_rgba_n_uv2_bt_4Bones* first = &vertices->back();
			first->nx = testNormal.x;
			first->ny = testNormal.y;
			first->nz = testNormal.z;
			first->sliced = 1.0f;

			vertices->push_back((*vertices)[pairs[i].second]);
			sVertex_xyz_rgba_n_uv2_bt_4Bones* second = &vertices->back();
			second->nx = testNormal.x;
			second->ny = testNormal.y;
			second->nz = testNormal.z;
			second->sliced = 1.0f;

			vertSize += 2;
			pairsSliceFace.push_back(std::make_pair<int, int>(vertSize - 1, vertSize - 2));
		}


		//average it based on all the pairs 
		pairsSize = pairsSliceFace.size();
		vertSize = vertices->size();
		glm::vec3 averagePos = glm::vec3(0);
		float divisionValue = pairsSize * 2;
		for (int i = 0; i < pairsSize; ++i)
		{
			cTriangle* newTri = new cTriangle();
			newTri->vertex_ID_0 = pairsSliceFace[i].first;
			newTri->vertex_ID_1 = pairsSliceFace[i].second;
			newTri->vertex_ID_2 = vertSize;

			averagePos.x += (*vertices)[pairsSliceFace[i].first].x;
			averagePos.y += (*vertices)[pairsSliceFace[i].first].y;
			averagePos.z += (*vertices)[pairsSliceFace[i].first].z;

			averagePos.x += (*vertices)[pairsSliceFace[i].second].x;
			averagePos.y += (*vertices)[pairsSliceFace[i].second].y;
			averagePos.z += (*vertices)[pairsSliceFace[i].second].z;

			triangles->push_back(*newTri);
		}

		averagePos = averagePos / divisionValue;
		finalVert->x = averagePos.x;
		finalVert->y = averagePos.y;
		finalVert->z = averagePos.z;
		finalVert->nx = testNormal.x;
		finalVert->ny = testNormal.y;
		finalVert->nz = testNormal.z;
		finalVert->sliced = 1.0f;

		vertices->push_back(*finalVert);

		//before reassigning the vertices make a copy of the object and call this method on it, but reverse the normal
		if (makeNewMesh)
		{
			cGameObject* theOtherHalf = new cGameObject();
			//somehow the mesh is getting more and more complex, to combat this we will test for type here and simply make
			switch (theObject->type)
			{
			case ObjectType::CHEESE:
			{
				sMeshDrawInfo meshInfo;
				meshInfo.name = "cheese.ply";
				meshInfo.scale = 2.0f;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("CheeseTexture.bmp", 1.0f));
				theOtherHalf->vecMeshes.push_back(meshInfo);
			}
			break;

			case ObjectType::SALAMI:
			{
				sMeshDrawInfo meshInfo;
				meshInfo.name = "meat.ply";
				meshInfo.scale = 2.0f;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("MeatTexture.bmp", 1.0f));
				theOtherHalf->vecMeshes.push_back(meshInfo);
			}
			break;

			case ObjectType::TOMATO:
			{
				sMeshDrawInfo meshInfo;
				meshInfo.name = "tomato.ply";
				meshInfo.scale = 2.0f;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("tomatoTexture.bmp", 1.0f));
				theOtherHalf->vecMeshes.push_back(meshInfo);
			}
			break;

			case ObjectType::BREADSLICE:
			{
				sMeshDrawInfo meshInfo;
				meshInfo.name = "breadLoaf.ply";
				meshInfo.scale = 2.0f;
				meshInfo.bDisableBackFaceCulling = true;
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("BreadLoafT.bmp", 1.0f));
				meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("breadInside.bmp", 0.0f));
				theOtherHalf->vecMeshes.push_back(meshInfo);
			}
			break;
			}
			//a new version of the original mesh

			theOtherHalf->type = theObject->type;
			pCuttingObject = theOtherHalf;
			theOtherHalf->setPosition(theObject->getPosition());
			//theOtherHalf->vecMeshes[0].scale = theObject->vecMeshes[0].scale;
			theOtherHalf->worldMatrix = theObject->worldMatrix;
			theOtherHalf->m_PhysicalProps.qOrientation = theObject->m_PhysicalProps.qOrientation;
			::g_vecGameObjects.push_back(theOtherHalf);

			::g_vecGameObjects.back()->Slice(false, pointOnPlane, testNormal * -1.0f);
			//::g_vecGameObjects.back()->setPosition(::g_vecGameObjects.back()->getPosition() + (testNormal * 0.5f));
			::g_vecGameObjects.back()->vecMeshes[0].bDisableBackFaceCulling = true;
			::g_vecGameObjects.back()->cuttable = true;
		}


		if (makeNewMesh)
		{
			theNewMesh.name = theOldMesh->name + std::to_string(newMeshID);
			theObject->vecMeshes[0].name = theObject->vecMeshes[0].name + std::to_string(newMeshID);
			newMeshID++;
			theObject->ReorientPositionAndVertices(vertices, triangles, true);
		}
		else
		{
			theNewMesh.name = theOldMesh->name + std::to_string(newMeshID);
			theObject->vecMeshes[0].name = theObject->vecMeshes[0].name + std::to_string(newMeshID);
			newMeshID++;
		}



		theNewMesh.pVertices = &(*vertices)[0];
		theNewMesh.numberOfVertices = vertices->size();
		theNewMesh.numberOfTriangles = triangles->size();
		theNewMesh.pTriangles = &(*triangles)[0];

		GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");
		g_pVAOManager->loadMeshIntoVAO(theNewMesh, sexyShaderID);

	}

	return meshInCut;

}

bool cGameObject::Slice(bool makeNewMesh, glm::vec3 pointOnPlane, glm::vec3 testNormal)
{
	return SliceP(makeNewMesh, pointOnPlane, testNormal, this);
}


glm::vec3 cGameObject::GetVelocity()
{
	return this->m_PhysicalProps.velocity;
}

void cGameObject::SetVelocity(glm::vec3 newVel)
{
	this->m_PhysicalProps.velocity = newVel;
}

void cGameObject::UpdatePhys(double time)
{
}

void cGameObject::RecalculateWorldMatrix()
{
	//recalulate the world matrix
	this->worldMatrix = glm::mat4x4(1.0f);

	glm::mat4 trans = glm::mat4x4(1.0f);
	trans = glm::translate(trans, this->getPosition());
	this->worldMatrix = this->worldMatrix * trans;

	glm::mat4 postRotQuat = glm::mat4(this->getQOrientation());
	this->worldMatrix = this->worldMatrix * postRotQuat;
	// The scale is relative to the original model
	glm::mat4 matScale = glm::mat4x4(1.0f);
	matScale = glm::scale(matScale,
		glm::vec3(this->vecMeshes[0].scale,
			this->vecMeshes[0].scale,
			this->vecMeshes[0].scale));
	this->worldMatrix = this->worldMatrix * matScale;

	this->inverseWorldMatrix = glm::inverse(worldMatrix);
}

void cGameObject::DeleteChildren(void)
{
	for (std::vector< cGameObject* >::iterator itChild = this->vec_pChildObjects.begin();
		itChild != this->vec_pChildObjects.end(); itChild++)
	{
		// Pointer not zero (0)?
		cGameObject* pTempChildObject = (*itChild);
		if (pTempChildObject != 0)
		{
			// Recursively delete all children's children (and so on)
			pTempChildObject->DeleteChildren();
			// Now delete this child
			delete pTempChildObject;
		}
	}
	// There's a vector, but nothing in it
	this->vec_pChildObjects.clear();
	return;
}

cGameObject* cGameObject::FindChildByFriendlyName(std::string name)
{
	for (std::vector<cGameObject*>::iterator itCGO = this->vec_pChildObjects.begin(); itCGO != this->vec_pChildObjects.end(); itCGO++)
	{
		if ((*itCGO)->friendlyName == name)
		{
			return (*itCGO);
		}
	}
	// Didn't find it.
	return NULL;
}

cGameObject* cGameObject::FindChildByID(unsigned int ID)
{
	for (std::vector<cGameObject*>::iterator itCGO = this->vec_pChildObjects.begin(); itCGO != this->vec_pChildObjects.end(); itCGO++)
	{
		if ((*itCGO)->getUniqueID() == ID)
		{
			return (*itCGO);
		}
	}
	// Didn't find it.
	return NULL;
}

void cGameObject::UnsetAllAnimationStates()
{
}



//glm::quat cGameObject::getFinalMeshQOrientation(void)
//{
//	return this->m_PhysicalProps.qOrientation * this->m_meshQOrientation;
//}

glm::quat cGameObject::getFinalMeshQOrientation(unsigned int meshID)
{	// Does NOT check for the index of the mesh!
	return this->m_PhysicalProps.qOrientation * this->vecMeshes[meshID].getQOrientation();
}

glm::quat cGameObject::getFinalMeshQOrientation(glm::quat &meshQOrientation)
{	// Does NOT check for the index of the mesh!
	return this->m_PhysicalProps.qOrientation * meshQOrientation;
}
