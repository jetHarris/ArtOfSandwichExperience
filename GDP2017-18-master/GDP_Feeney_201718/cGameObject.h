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

class cSimpleAssimpSkinnedMesh;	// Forward declare
class cAnimationState;			// Forward declare 

class sVertex_xyz_rgba_n_uv2_bt_4Bones;
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
//	cGameObject(const cGameObject &obj);  // copy constructor

	//***** from the iPhysicalObject interface ********************
	virtual void SetPhysState( cPhysicalProperties &PhysState );
	virtual cPhysicalProperties GetPhysState( void );
	virtual void GetPhysState( cPhysicalProperties &PhysState );
	//*************************************************************
	glm::vec3 getPosition(void);
	void setPosition(glm::vec3 pos);
	// bOverwritePositionToo effectively stops the object if the "past position" is being used
	void	  overwritePotition( glm::vec3 newPosition, bool bOverwiteOldPositionToo = true );
	glm::quat getQOrientation(void);		// Post-rotation
	void      overwriteQOrientation( glm::quat newOrientation );
	void      overwriteQOrientationEuler( glm::vec3 newOrientationEuler, bool bIsDegrees = true );
	void      adjQOrientation( glm::quat newOrientation );
	void      adjQOrientationEuler( glm::vec3 newOrientationEuler, bool bIsDegrees = true );

	//****************************************************************************************

	bool eraseMeshPart(bool makeNewMesh, glm::vec3 pointOnPlane, glm::vec3 testNormal);
	bool RightSideOfPlane(sVertex_xyz_rgba_n_uv2_bt_4Bones&const vertex, glm::vec3&const pointOnPlane, glm::vec3&const testNormal);
	void CleanOldVertices(std::vector<sVertex_xyz_rgba_n_uv2_bt_4Bones>* vertices, std::vector<cTriangle>* triangles);
	void ReorientPositionAndVertices(std::vector<sVertex_xyz_rgba_n_uv2_bt_4Bones>* vertices, std::vector<cTriangle>* triangles, bool makePhysics);
	glm::vec3 FindIntersectionPoint(sVertex_xyz_rgba_n_uv2_bt_4Bones&const vertexIn, sVertex_xyz_rgba_n_uv2_bt_4Bones&const vertexOut, glm::vec3&const pointOnPlane, glm::vec3&const testNormal);
	glm::vec3 AveragePoint(sVertex_xyz_rgba_n_uv2_bt_4Bones&const vertex1, sVertex_xyz_rgba_n_uv2_bt_4Bones&const vertex2);

	//for threading the slice
	void loadMeshIntoVAONow();
	bool slicingFinished = false;
	std::vector<cMesh> loadMeshVector;
	bool makeNewMesh;
	glm::vec3 pointOnPlane;
	glm::vec3 testNormal;
	HANDLE slicingHandle;
	DWORD slicingAddress;
	std::string newMeshName;



	std::string friendlyName;

	void calcXYZDepths(float &xIn, float &yIn, float &zIn);
	inline unsigned int getUniqueID(void) { return this->m_UniqueID; }

	iDebugRenderer* pDebugRenderer;
	//glm::vec3 animationPosition;

	bool bDiscardTexture = false;
	bool isLookedAt = false;

	glm::vec3 GetVelocity();
	void SetVelocity(glm::vec3 newVel);
	void UpdatePhys(double time);
	glm::mat4 worldMatrix;
	glm::mat4 inverseWorldMatrix;
	void RecalculateWorldMatrix();
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
	glm::quat getFinalMeshQOrientation(unsigned int meshID);
	glm::quat getFinalMeshQOrientation(glm::quat &meshQOrientation);


	bool bIsVisible;	// If false, any meshes are NOT drawn (not child objects could still be visible)

	// Our "child" objects
	std::vector< cGameObject* > vec_pChildObjects;
	void DeleteChildren(void);
	// Returns NULL if not found
	cGameObject* FindChildByFriendlyName( std::string name );
	cGameObject* FindChildByID( unsigned int ID );

	void UnsetAllAnimationStates();


	// Used when there is only one game object (like with text), but we're drawing it many times
	//void pushRenderingState(void);
	//// Ignores call if nothing on stack
	//void popRenderingState(void);

	// If NULL, then object ISN'T a skinned mesh
	cSimpleAssimpSkinnedMesh*	pSimpleSkinnedMesh;	
	cAnimationState*			pAniState;			
	cPhysicalProperties m_PhysicalProps;
private:
	unsigned int m_UniqueID;
	// Used when creating objects
	static unsigned int m_nextUniqueID;

	// All the properties of a physical object 
	

};


#endif
