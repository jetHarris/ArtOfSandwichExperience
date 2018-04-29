#pragma once
#include <iPhysicsWorld.h>
#include <vector>
#include "cBulletRigidBody.h"
#include "cBulletIntegrator.h"
//#include "BulletInverseDynamics/IDConfigBuiltin.hpp"
//#include "bt"
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include <iConstraint.h>

namespace nPhysics
{
	class cBulletPhysicsWorld : public iPhysicsWorld
	{
	public:
		virtual ~cBulletPhysicsWorld();
		cBulletPhysicsWorld();
		cBulletPhysicsWorld(btVector3 gravity);
		cBulletPhysicsWorld(glm::vec3 max, glm::vec3 min);

		virtual void TimeStep(float deltaTime);
		void Collisions();

		virtual void AddRigidBody(iRigidBody* rigidBody);
		virtual void RemoveRigidBody(iRigidBody* rigidBody);

		virtual void SetDebugRenderer(iDebugRenderer* debugRenderer);
		virtual void RenderDebug();
		virtual void Collide(iRigidBody* first, iRigidBody* second);
		virtual void CollideSphereSphere(iRigidBody* first, iRigidBody* second);
		virtual void CollideSpherePlane(iRigidBody* first, iRigidBody* second);

		virtual bool AddConstraint(iConstraint* constraint);
		virtual bool RemoveConstraint(iConstraint* constraint);

		virtual void RayCast(glm::vec3 start, glm::vec3 end);

	private:
		
		std::vector<iRigidBody*> rigidBodies;
		float deltaTime;
		cBulletIntegrator integrator;
		double totalTime;

		glm::vec3 worldMax;
		glm::vec3 worldMin;

		btDiscreteDynamicsWorld* dynamicsWorld;
		btAlignedObjectArray<btCollisionShape*> collisionShapes;
		btSequentialImpulseConstraintSolver* solver;
		btBroadphaseInterface* overlappingPairCache;
		btCollisionDispatcher* dispatcher;
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionWorld* collisionWorld;
		//btBroadphaseInterface* broadPhase;
	};
}