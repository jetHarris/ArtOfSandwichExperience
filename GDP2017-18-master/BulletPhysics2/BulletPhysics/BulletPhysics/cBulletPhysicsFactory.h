#pragma once
#include <iPhysicsFactory.h>
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

namespace nPhysics
{
	class cBulletPhysicsFactory : public iPhysicsFactory
	{
	public:
		virtual ~cBulletPhysicsFactory();

		virtual iPhysicsWorld* CreateWorld();

		//bodies
		virtual iRigidBody* CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape);

		//shapes
		virtual iShape* CreateSphere(float radius);
		virtual iShape* CreatePlane(const glm::vec3& normal, float planeConst);
		virtual iShape* CreateBox(const glm::vec3& boxHalfWidths);
		virtual iShape* CreateCylinder(const glm::vec3& boxHalfWidths);
		virtual iShape* CreateCone(float height, float radius);
		virtual iShape* CreateConvexHull(float * points, int numPoints, int stride);
		virtual iShape* CreateConvexHull(float * points, int numPoints);
		virtual iShape* CreateConvexHull();

		//contraints
		virtual iConstraint* CreateBallAndSocketConstraint(iRigidBody* rb, const glm::vec3& pivot);
		virtual iConstraint* CreateBallAndSocketConstraint(iRigidBody* rbA, const glm::vec3& pivotA, iRigidBody* rbB, const glm::vec3& pivotB);
		virtual iConstraint* CreateHingeConstraint(iRigidBody* rb, const glm::vec3& pivot, const glm::vec3& axis, float lowLimit, float highLimit);
		virtual iConstraint* CreateFixedConstraint(iRigidBody* rbA, iRigidBody* rbB, glm::mat4 matA, glm::mat4 matB);
		virtual iConstraint* CreateGearConstraint(iRigidBody* rbA, iRigidBody* rbB, const glm::vec3& axisInA, const glm::vec3& axisInB, float ratio = 1.f);
		virtual iConstraint* CreateConeConstraint(iRigidBody* rbA, iRigidBody* rbB, glm::mat4 rbAFrame, glm::mat4 rbBFrame);

		virtual iConstraint* Create6DegreesConstraint(iRigidBody* rbA, glm::mat4 rbAFrame);
	};
}