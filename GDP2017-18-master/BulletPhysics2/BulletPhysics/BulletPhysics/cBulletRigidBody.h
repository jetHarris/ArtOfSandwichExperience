#pragma once
#include <iRigidBody.h>
#include <sRigidBodyDesc.h>
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

namespace nPhysics
{
	class cPhysicsWorld;
	class cBulletRigidBody : public iRigidBody
	{
	public:
		cBulletRigidBody(const sRigidBodyDesc& desc, iShape* shape);
		virtual ~cBulletRigidBody();

		virtual iShape* GetShape();

		virtual void GetTransform(glm::mat4& transformOut);
		virtual void GetPosition(glm::vec3& positionOut);
		virtual void GetRotation(glm::vec3& rotationOut);
		virtual void SetStatic(bool isStat);
		virtual void GetVelocity(glm::vec3& velocityOut);
		virtual void SetVelocity(glm::vec3& velocityIn);
		virtual void Push(glm::vec3 accel);
		virtual void GetOrientation(glm::quat &orientation);
		virtual void SetOrientation(glm::vec3 rotation);
		virtual void SetOrientation(glm::quat rotation);
		virtual void GetColliding(bool& colliding);
		virtual void SetColliding(bool colliding);
		virtual void GetRayCasted(bool& rayCastedOut);
		virtual void SetRayCasted(bool rayCastedIn);
		virtual void GetRayCastPos(glm::vec3& positionOut);
		virtual void SetRayCastPos(glm::vec3 positionIn);
		virtual void ZeroOutRotationalVel();
		virtual void GetRotationalVel(glm::vec3& velocityOut);
		virtual void SetRotationalVel(glm::vec3& velocityIn);
		bool isStatic;

		btRigidBody* body;
		btMotionState* motionState;
		btCollisionShape* collisionShape;
		btTransform* transform;

	private:
		friend class cBulletPhysicsWorld;
		iShape* shape;
		glm::vec3 position;
		glm::vec3 positionOld;
		glm::vec3 velocity;
		glm::vec3 velocityOld;
		glm::quat rotation;
		glm::vec3 acceleration;
		glm::quat orientation;
		glm::quat orientationVelocity;
		glm::vec3 lastRayCastPos;
		float mass;
		bool colliding;
		float collidingCounter;
		bool rayCasted;


		
	};
}