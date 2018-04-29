#ifndef BULLETCONSTRAINT_HG
#define BULLETCONSTRAINT_HG

#include <iConstraint.h>
#include "../BulletPhysics/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"
#include "../BulletPhysics/BulletDynamics/ConstraintSolver/btHingeConstraint.h"
#include "../BulletPhysics/BulletDynamics/ConstraintSolver/btFixedConstraint.h"

namespace nPhysics
{
	class cBulletRigidBody;
	class cBulletPhysicsWorld;

	class cBulletConstraint : public iConstraint
	{
	public:
		virtual ~cBulletConstraint()
		{
			delete constraint;
		}

		friend class cBulletPhysicsWorld;
		cBulletConstraint(eConstraintType constraintType, btTypedConstraint* constraintIn) : iConstraint(constraintType), constraint(constraintIn)
		{
			constraint->setUserConstraintPtr(0);
		}

		btTypedConstraint* constraint;
	};

	class cBallAndSocketConstraint : public cBulletConstraint
	{
	public:
		cBallAndSocketConstraint(cBulletRigidBody* rb, const btVector3& pivot);
		cBallAndSocketConstraint(cBulletRigidBody* rbA, const btVector3& pivotA, cBulletRigidBody* rgB, const btVector3& pivotB);
		virtual ~cBallAndSocketConstraint();
	};

	class c6DegreesOfFreedomConstraint : public cBulletConstraint
	{
	public:
		c6DegreesOfFreedomConstraint::c6DegreesOfFreedomConstraint(cBulletRigidBody * rb, const btTransform & rbAFrame);
		virtual ~c6DegreesOfFreedomConstraint();
	};

	class cHingeConstraint : public cBulletConstraint
	{
	public:
		cHingeConstraint(cBulletRigidBody* rb, const btVector3& pivot, const btVector3& axis, float lowLimit, float highLimit);
		virtual ~cHingeConstraint();
	};

	class cFixedConstraint : public cBulletConstraint
	{
	public:
		cFixedConstraint(cBulletRigidBody* rbA, cBulletRigidBody* rbB, const btTransform& frameInA, const btTransform& frameInB);
		virtual ~cFixedConstraint();
	};

	class cGearConstraint : public cBulletConstraint
	{
	public:
		cGearConstraint(cBulletRigidBody* rbA, cBulletRigidBody* rbB, const btVector3& axisInA, const btVector3& axisInB, btScalar ratio = 1.0f);
		virtual ~cGearConstraint();
	};

	class cConeConstraint : public cBulletConstraint
	{
	public:
		cConeConstraint(cBulletRigidBody* rbA, cBulletRigidBody* rbB, const btTransform& rbAFrame, const btTransform& rbBframe);
		virtual ~cConeConstraint();
	};
}
#endif // !BULLETCONSTRAINT_HG

