#pragma once
#include "iRigidBody.h"
#include "sRigidBodyDesc.h"
#include "iShape.h"
#include "iPhysicsWorld.h"
#include "iConstraint.h"

namespace nPhysics
{
	class iPhysicsFactory
	{
	public:
		virtual ~iPhysicsFactory() {}

		virtual iPhysicsWorld* CreateWorld() = 0;

		virtual iRigidBody* CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape) = 0;

		virtual iShape* CreateSphere(float radius) = 0;
		virtual iShape* CreatePlane(const glm::vec3& normal, float planeConst) = 0;
		virtual iShape* CreateBox(const glm::vec3& boxHalfWidths) = 0;
		virtual iShape* CreateCylinder(const glm::vec3& cylinderHalfWidths) = 0;
		virtual iShape* CreateCone(float height, float radius) = 0;
		virtual iShape* CreateConvexHull(float * points, int numPoints, int stride) = 0;
		virtual iShape* CreateConvexHull(float * points, int numPoints) = 0;
		virtual iShape* CreateConvexHull() = 0;

		virtual iConstraint* CreateBallAndSocketConstraint(iRigidBody* rb, const glm::vec3& pivot) = 0;
		virtual iConstraint* CreateBallAndSocketConstraint(iRigidBody* rbA, const glm::vec3& pivotA, iRigidBody* rbB, const glm::vec3& pivotB) = 0;
		virtual iConstraint* CreateHingeConstraint(iRigidBody* rb, const glm::vec3& pivot, const glm::vec3& axis, float lowLimit, float highLimit) = 0;
		virtual iConstraint* CreateFixedConstraint(iRigidBody* rbA, iRigidBody* rbB, glm::mat4 matA, glm::mat4 matB) = 0;
		virtual iConstraint* CreateGearConstraint(iRigidBody* rbA, iRigidBody* rbB, const glm::vec3& axisInA, const glm::vec3& axisInB, float ratio = 1.f) = 0;
		virtual iConstraint* CreateConeConstraint(iRigidBody* rbA, iRigidBody* rbB, glm::mat4 rbAFrame, glm::mat4 rbBFrame) = 0;
		virtual iConstraint* Create6DegreesConstraint(iRigidBody* rbA, glm::mat4 rbAFrame) = 0;
	};
}