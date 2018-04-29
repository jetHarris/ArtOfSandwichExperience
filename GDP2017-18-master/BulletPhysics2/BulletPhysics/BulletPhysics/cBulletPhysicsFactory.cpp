#include "cBulletPhysicsFactory.h"
#include "cBulletRigidBody.h"
#include "BulletShapes.h"
#include "cBulletPhysicsWorld.h"
#include "nConvert.h"
#include "BulletConstraints.h"

namespace nPhysics
{
	cBulletPhysicsFactory::~cBulletPhysicsFactory() {}

	iPhysicsWorld* cBulletPhysicsFactory::CreateWorld()
	{
		return new cBulletPhysicsWorld();
	}

	iRigidBody* cBulletPhysicsFactory::CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape)
	{
		return new cBulletRigidBody(desc, shape);
	}

	iShape* cBulletPhysicsFactory::CreateSphere(float radius)
	{
		return new cBulletSphereShape(radius);
	}

	iShape * cBulletPhysicsFactory::CreatePlane(const glm::vec3 & normal, float planeConst)
	{
		return new cBulletPlaneShape(normal, planeConst);
	}

	iShape * cBulletPhysicsFactory::CreateBox(const glm::vec3 & boxHalfWidths)
	{
		return new cBulletBoxShape(boxHalfWidths);
	}

	iShape * cBulletPhysicsFactory::CreateCylinder(const glm::vec3 & cylinderHalfWidths)
	{
		return new cBulletCylinderShape(cylinderHalfWidths);
	}

	iShape * cBulletPhysicsFactory::CreateCone(float height, float radius)
	{
		return new cBulletConeShape(height,radius);
	}

	iShape * cBulletPhysicsFactory::CreateConvexHull(float * points, int numPoints, int stride)
	{
		return new cBulletConvexHullShape(points, numPoints, stride);
	}

	iShape * cBulletPhysicsFactory::CreateConvexHull(float * points, int numPoints)
	{
		return new cBulletConvexHullShape(points, numPoints);
	}

	iShape * cBulletPhysicsFactory::CreateConvexHull()
	{
		return new cBulletConvexHullShape();
	}

	iConstraint* cBulletPhysicsFactory::CreateBallAndSocketConstraint(iRigidBody* rb, const glm::vec3& pivot)
	{
		cBulletRigidBody* bodyA = dynamic_cast<cBulletRigidBody*>(rb);

		if (!bodyA) return 0;
		return new cBallAndSocketConstraint(bodyA, nConvert::ToBullet(pivot));
	} 

	iConstraint* cBulletPhysicsFactory::CreateBallAndSocketConstraint(iRigidBody* rbA, const glm::vec3& pivotA, iRigidBody* rbB, const glm::vec3& pivotB)
	{
		cBulletRigidBody* bodyA = dynamic_cast<cBulletRigidBody*>(rbA);
		cBulletRigidBody* bodyB = dynamic_cast<cBulletRigidBody*>(rbB);

		if (!bodyA) return 0;
		if (!bodyB) return 0;

		return new cBallAndSocketConstraint(bodyA, nConvert::ToBullet(pivotA), bodyB, nConvert::ToBullet(pivotB));
	} 

	iConstraint* cBulletPhysicsFactory::CreateHingeConstraint(iRigidBody* rb, const glm::vec3& pivot, const glm::vec3& axis, float lowLimit, float highLimit)
	{
		cBulletRigidBody* bodyB = dynamic_cast<cBulletRigidBody*>(rb);

		if (!bodyB) return 0;
		return new cHingeConstraint(bodyB, nConvert::ToBullet(pivot), nConvert::ToBullet(axis), lowLimit, highLimit);
	}

	iConstraint* cBulletPhysicsFactory::CreateFixedConstraint(iRigidBody* rbA, iRigidBody* rbB, glm::mat4 matA, glm::mat4 matB)
	{
		cBulletRigidBody* bodyA = dynamic_cast<cBulletRigidBody*>(rbA);
		cBulletRigidBody* bodyB = dynamic_cast<cBulletRigidBody*>(rbB);

		if (!bodyA) return 0;
		if (!bodyB) return 0;

		btTransform tempA = nConvert::ToBullet(matA);
		btTransform tempB = nConvert::ToBullet(matB);

		return new cFixedConstraint(bodyA, bodyB, tempA, tempB);
	} 


	iConstraint* cBulletPhysicsFactory::CreateGearConstraint(iRigidBody* rbA, iRigidBody* rbB, const glm::vec3& axisInA, const glm::vec3& axisInB, float ratio)
	{
		cBulletRigidBody* bodyA = dynamic_cast<cBulletRigidBody*>(rbA);
		cBulletRigidBody* bodyB = dynamic_cast<cBulletRigidBody*>(rbB);

		if (!bodyA) return 0;
		if (!bodyB) return 0;

		return new cGearConstraint(bodyA, bodyB, nConvert::ToBullet(axisInA), nConvert::ToBullet(axisInB), ratio);
	} 


	iConstraint* cBulletPhysicsFactory::CreateConeConstraint(iRigidBody* rbA, iRigidBody* rbB, glm::mat4 rbAFrame, glm::mat4 rbBFrame)
	{
		cBulletRigidBody* bodyA = dynamic_cast<cBulletRigidBody*>(rbA);
		cBulletRigidBody* bodyB = dynamic_cast<cBulletRigidBody*>(rbB);

		if (!bodyA) return 0;
		if (!bodyB) return 0;

		return new cConeConstraint(bodyA, bodyB, nConvert::ToBullet(rbAFrame), nConvert::ToBullet(rbBFrame));
	}

	iConstraint * cBulletPhysicsFactory::Create6DegreesConstraint(iRigidBody * rbA, glm::mat4 rbAFrame)
	{
		cBulletRigidBody* bodyB = dynamic_cast<cBulletRigidBody*>(rbA);

		if (!bodyB) return 0;

		return new c6DegreesOfFreedomConstraint(bodyB, nConvert::ToBullet(rbAFrame));
	}
	
}