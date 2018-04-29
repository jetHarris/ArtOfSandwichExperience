#pragma once
#include "iRigidBody.h"
#include "iDebugRenderer.h"
#include "iConstraint.h"

namespace nPhysics
{
	class iPhysicsWorld
	{
	public:
		virtual ~iPhysicsWorld() {}

		virtual void TimeStep(float deltaTime) = 0;

		virtual void AddRigidBody(iRigidBody* rigidBody) = 0;
		virtual void RemoveRigidBody(iRigidBody* rigidBody) = 0;

		virtual void SetDebugRenderer(iDebugRenderer* debugRenderer) = 0;
		virtual void RenderDebug() = 0;
		virtual void Collide(iRigidBody* first, iRigidBody* second) = 0;

		virtual bool AddConstraint(iConstraint* constraint) = 0;
		virtual bool RemoveConstraint(iConstraint* constraint) = 0;
		virtual void RayCast(glm::vec3 start, glm::vec3 end) = 0;
	};
}