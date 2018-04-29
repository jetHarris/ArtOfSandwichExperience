#include "BulletConstraints.h"
#include "cBulletRigidBody.h"


namespace nPhysics
{
	cBallAndSocketConstraint::cBallAndSocketConstraint(cBulletRigidBody* rb, const btVector3& pivot)
		: cBulletConstraint(CONSTRAINT_TYPE_BALL_AND_SOCKET, new btPoint2PointConstraint(*rb->body, pivot))
	{

	}

	cBallAndSocketConstraint::cBallAndSocketConstraint(cBulletRigidBody* rbA, const btVector3& pivotA, cBulletRigidBody* rbB, const btVector3& pivotB)
		: cBulletConstraint(CONSTRAINT_TYPE_BALL_AND_SOCKET, new btPoint2PointConstraint(*(rbA->body), *(rbB->body), pivotA, pivotB))
	{

	}

	cBallAndSocketConstraint::~cBallAndSocketConstraint()
	{

	}

	cHingeConstraint::cHingeConstraint(cBulletRigidBody * rb, const btVector3 & pivot, const btVector3 & axis, float lowLimit, float highLimit)
		:cBulletConstraint(CONSTRAINT_TYPE_HINGE, new btHingeConstraint(*rb->body, pivot, axis))
	{
	}
	cHingeConstraint::~cHingeConstraint()
	{
	}


	cFixedConstraint::cFixedConstraint(cBulletRigidBody * rbA, cBulletRigidBody * rbB, const btTransform & frameInA, const btTransform & frameInB)
		:cBulletConstraint(CONSTRAINT_TYPE_FIXED, new btFixedConstraint(*rbA->body, *rbB->body, frameInA, frameInB))
	{
	}

	cFixedConstraint::~cFixedConstraint()
	{
	}



	cGearConstraint::cGearConstraint(cBulletRigidBody * rbA, cBulletRigidBody * rbB, const btVector3 & axisInA, const btVector3 & axisInB, btScalar ratio)
		: cBulletConstraint(CONSTRAINT_TYPE_GEAR, new btGearConstraint(*rbA->body, *rbB->body,axisInA, axisInB, ratio))
	{
	}

	cGearConstraint::~cGearConstraint()
	{
	}

	cConeConstraint::cConeConstraint(cBulletRigidBody * rbA, cBulletRigidBody * rbB, const btTransform & rbAFrame, const btTransform & rbBframe)
		: cBulletConstraint(CONSTRAINT_TYPE_CONE, new btConeTwistConstraint(*rbA->body,*rbB->body, rbAFrame, rbBframe))
	{
	}

	cConeConstraint::~cConeConstraint()
	{
	}

	c6DegreesOfFreedomConstraint::c6DegreesOfFreedomConstraint(cBulletRigidBody * rb, const btTransform & rbAFrame)
		:cBulletConstraint(CONSTRAINT_TYPE_DEGREES, new btGeneric6DofConstraint(*rb->body, rbAFrame, true))
	{
		((btGeneric6DofConstraint*)constraint)->setLinearLowerLimit(btVector3(-999,10,-999));
		((btGeneric6DofConstraint*)constraint)->setLinearUpperLimit(btVector3(999, 10, 999));
		((btGeneric6DofConstraint*)constraint)->setAngularLowerLimit(btVector3(-999, -999, -999));
		((btGeneric6DofConstraint*)constraint)->setAngularUpperLimit(btVector3(999, 999, 999));
	}

	c6DegreesOfFreedomConstraint::~c6DegreesOfFreedomConstraint()
	{
	}

}