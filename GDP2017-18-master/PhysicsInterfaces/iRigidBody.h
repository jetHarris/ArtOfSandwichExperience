#pragma once
#include <glm/game_math.h>
#include "iShape.h"

namespace nPhysics
{
	class iRigidBody
	{
	public:
		virtual ~iRigidBody() {}

		virtual iShape* GetShape() = 0;

		virtual void GetTransform(glm::mat4& transformOut) = 0;
		virtual void GetPosition(glm::vec3& positionOut) = 0;
		virtual void GetRotation(glm::vec3& rotationOut) = 0;
		virtual void GetVelocity(glm::vec3& velocityOut) = 0;
		virtual void SetVelocity(glm::vec3& velocityIn) = 0;
		virtual void GetRotationalVel(glm::vec3& velocityOut) = 0;
		virtual void SetRotationalVel(glm::vec3& velocityIn) = 0;
		

		virtual void GetColliding(bool& collidingOut) = 0;
		virtual void SetColliding(bool collidingIn) = 0;

		virtual void GetRayCastPos(glm::vec3& positionOut) = 0;
		virtual void SetRayCastPos(glm::vec3 positionIn) = 0;

		virtual void GetRayCasted(bool& rayCastedOut) = 0;
		virtual void SetRayCasted(bool rayCastedIn) = 0;

		virtual void SetStatic(bool isStat) = 0;
		virtual void Push(glm::vec3 accel) = 0;
		virtual void GetOrientation(glm::quat &orientation) = 0;
		virtual void SetOrientation(glm::vec3 rotation) = 0;
		virtual void SetOrientation(glm::quat rotation) = 0;
		virtual void ZeroOutRotationalVel() = 0;
		
		//virtual void SetPosition(glm::vec3& positionIn) = 0;

	};
}