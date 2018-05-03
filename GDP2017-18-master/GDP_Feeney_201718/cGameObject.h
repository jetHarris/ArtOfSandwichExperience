#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <BulletPhysics\BulletPhysics\cBulletPhysicsFactory.h>
#include <queue>

#include "iDebugRenderer.h"

#include "Physics/cPhysicalProperties.h"
#include "Physics/iPhysicalObject.h"	// For the access to things that will update the positions, etc.

#include "sMeshDrawInfo.h"
#include "cMesh.h"
#include <Windows.h>

class sVertex_xyz_rgba_n_uv2;
class cTriangle;

enum ObjectType
{
	MISC,
	SALAMI,
	CHEESE,
	TOMATO,
	OLIVE,
	BREADLOAF,
	BREADSLICE,
	KNIFE,
	TOASTEDBREADSLICE
};

class cGameObject : public iPhysicalObject
{
public:
	cGameObject();		// constructor
	~cGameObject();		// destructor

	//methods

	//***** from the iPhysicalObject interface ********************
	virtual void SetPhysState( cPhysicalProperties &PhysState );
	virtual cPhysicalProperties GetPhysState( void );
	virtual void GetPhysState( cPhysicalProperties &PhysState );
	//*************************************************************
	glm::vec3 getPosition(void);
	void setPosition(glm::vec3 pos);

	void	  overwritePosition( glm::vec3 newPosition, bool bOverwiteOldPositionToo = true );
	glm::quat getQOrientation(void);		// Post-rotation
	void      overwriteQOrientation( glm::quat newOrientation );
	void      overwriteQOrientationEuler( glm::vec3 newOrientationEuler, bool bIsDegrees = true );
	void      adjQOrientation( glm::quat newOrientation );
	void      adjQOrientationEuler( glm::vec3 newOrientationEuler, bool bIsDegrees = true );

	//****************************************************************************************

	//Divides one mesh into 2 based on a slicing plane
	bool Slice(bool makeNewMesh, glm::vec3 pointOnPlane, glm::vec3 testNormal);
	//detects is a vertex is on one side of a plane or the other. Used in Slice()
	bool RightSideOfPlane(sVertex_xyz_rgba_n_uv2&const vertex, glm::vec3&const pointOnPlane, glm::vec3&const testNormal);
	//a method that goes through the object and deletes unused vertices
	void CleanOldVertices(std::vector<sVertex_xyz_rgba_n_uv2>* vertices, std::vector<cTriangle>* triangles);
	//moves all vertices of a mesh after a change of origin for the object
	void ReorientPositionAndVertices(std::vector<sVertex_xyz_rgba_n_uv2>* vertices, std::vector<cTriangle>* triangles, bool makePhysics);
	//finds the intersection point between a line and a plane
	glm::vec3 FindIntersectionPoint(sVertex_xyz_rgba_n_uv2&const vertexIn, sVertex_xyz_rgba_n_uv2&const vertexOut, glm::vec3&const pointOnPlane, glm::vec3&const testNormal);
	//gets the average values between 2 vertices
	glm::vec3 AveragePoint(sVertex_xyz_rgba_n_uv2&const vertex1, sVertex_xyz_rgba_n_uv2&const vertex2);

	void calcXYZDepths(float &xIn, float &yIn, float &zIn);

	glm::quat getFinalMeshQOrientation(unsigned int meshID);
	glm::quat getFinalMeshQOrientation(glm::quat &meshQOrientation);

	inline unsigned int getUniqueID(void) { return this->m_UniqueID; }
	glm::vec3 GetVelocity();
	void SetVelocity(glm::vec3 newVel);
	void UpdatePhys(double time);
	void RecalculateWorldMatrix();

	

	
	

	std::string friendlyName;
	bool bDiscardTexture = false;
	bool isLookedAt = false;


	glm::mat4 worldMatrix;
	glm::mat4 inverseWorldMatrix;
	
	bool physics = false;
	nPhysics::iRigidBody* rBody;
	glm::vec3 rBodyOffset;
	glm::quat rBodyOrientationOffset;
	bool cuttable = false;
	bool lookable = true;
	bool spawner = false;
	int spawnerType;
	ObjectType type;
	bool castShadow = true;
	bool useLighting = true;


	// Mesh information (if drawn)
	// Note: Meshes have a separate orientation and offset from 
	//	the object, in case you want the mesh(es) to be
	//	loaded in different alignment from the game object. 
	//  If the object alignment is the same as the mesh
	//	alignment, then don't set the orientation and offset
	//	in the mesh information.

	std::vector<sMeshDrawInfo> vecMeshes;


	bool bIsVisible;
	cPhysicalProperties m_PhysicalProps;


	//for threading the slice
	//unused currently as the threaded version is disable
	///////////////////////////////////////////////////////
	void loadMeshIntoVAONow();
	bool slicingFinished = false;
	std::vector<cMesh> loadMeshVector;
	bool makeNewMesh;
	glm::vec3 pointOnPlane;
	glm::vec3 testNormal;
	HANDLE slicingHandle;
	DWORD slicingAddress;
	std::string newMeshName;
	///////////////////////////////////////////////////////
private:
	unsigned int m_UniqueID;
	// Used when creating objects
	static unsigned int m_nextUniqueID;	

};


#endif
