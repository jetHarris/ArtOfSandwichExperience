#include "cBulletRigidBody.h"
#include "BulletShapes.h"
#include "nConvert.h"

namespace nPhysics
{
	cBulletRigidBody::cBulletRigidBody(const sRigidBodyDesc& desc, iShape* shape)
		: shape(shape)
		, position(desc.Position)
		, velocity(desc.Velocity)
		, mass(desc.Mass)
		, rotation(desc.Rotation)
		, isStatic(false)
		, acceleration(glm::vec3(0,0,0))
		, orientationVelocity(glm::quat())
		, colliding(false)
		, collidingCounter(0.0f)
		, rayCasted(false)
		, lastRayCastPos(glm::vec3(0))
		
	{
		
		//body->setLinearVelocity(btVector3(desc.Velocity.x, desc.Velocity.y, desc.Velocity.z));*/


		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
		btCollisionShape* colShape = 0;
		if (shape->GetShapeType() == SHAPE_TYPE_PLANE)
		{
			isStatic = true;
			//glm::vec3 normal;
			/*(cBulletPlaneShape*)shape)->
			(cBulletPlaneShape*)shape)->GetPlaneNormal(normal);
			colShape = cBulletPlaneShape(, planeConst, pointOnPlane);*/

			glm::vec3 planeConst = desc.Position + ((cBulletPlaneShape*)shape)->normal * 30.0f;

			((cBulletPlaneShape*)shape)->bulletShape = 
				new btStaticPlaneShape(nConvert::ToBullet(((cBulletPlaneShape*)shape)->normal), 0);

			colShape = ((cBulletPlaneShape*)shape)->bulletShape;

			//(cBulletPlaneShape*)shape)->bulletShape = new btStaticPlaneShape(nConvert::ToBullet((cBulletPlaneShape*)shape)->)), glm::dot(normal, pointOnPlane));
		}
		else
		{
			colShape = ((cBulletSphereShape*)shape)->bulletShape;
		}

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setOrigin(nConvert::ToBullet(desc.Position));
		startTransform.setIdentity();

		btScalar mass(desc.Mass);

		//maybe use shape to tell about this one
		if (mass == 0.f || isStatic)
		{
			mass = 0.0f;
			isStatic = true;
		}

		btVector3 localInertia(0, 0, 0);
		if (!isStatic)
			colShape->calculateLocalInertia(mass, localInertia);

		//startTransform.setOrigin(btVector3(2, 10, 0));
		startTransform.setOrigin(btVector3(desc.Position.x, desc.Position.y, desc.Position.z));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		motionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);
		body = new btRigidBody(rbInfo);

		body->setUserPointer(this);

	}
	cBulletRigidBody::~cBulletRigidBody()
	{
		body->setUserPointer(0);
		delete body;
		delete motionState;
		delete shape;
		shape = 0;
	}

	iShape* cBulletRigidBody::GetShape()
	{
		return shape;
	}

	void cBulletRigidBody::GetPosition(glm::vec3& positionOut)
	{
		//positionOut = position;
		nConvert::ToGlm(body->getCenterOfMassPosition(), positionOut);
	}
	void cBulletRigidBody::GetRotation(glm::vec3& rotationOut)
	{
		rotationOut = glm::eulerAngles(rotation);
	}
	void cBulletRigidBody::SetStatic(bool isStat)
	{
		isStatic = isStat;
	}

	void cBulletRigidBody::SetOrientation(glm::vec3 rotation)
	{
		/*btQuaternion rotationIn(rotation.x, rotation.y, rotation.z);
		btTransform currentTransform = this->body->getWorldTransform();
		currentTransform.setRotation(rotationIn);
		*/
		btTransform tr = this->body->getWorldTransform();
		//tr.setIdentity();
		btQuaternion quat;
		quat.setEuler(glm::degrees(rotation.x), glm::degrees(rotation.y), glm::degrees(rotation.z)); //or quat.setEulerZYX depending on the ordering you want
		tr.setRotation(quat);
		this->body->setCenterOfMassTransform(tr);
	}
	void cBulletRigidBody::SetOrientation(glm::quat rotation)
	{
		btTransform tr = this->body->getWorldTransform();
		btQuaternion quat = nConvert::ToBullet(rotation);
		tr.setRotation(quat);
		this->body->setCenterOfMassTransform(tr);
	}
	void cBulletRigidBody::GetVelocity(glm::vec3 & velocityOut)
	{
		velocityOut = nConvert::ToGlm(this->body->getLinearVelocity());
	}
	void cBulletRigidBody::SetVelocity(glm::vec3 & velocityIn)
	{
		body->activate(true);
		this->body->setLinearVelocity(nConvert::ToBullet(velocityIn));
	}
	void cBulletRigidBody::Push(glm::vec3 accel)
	{
		//this->acceleration = accel;
		//body->applyImpulse(nConvert::ToBullet(accel),btVector3(0,0,0));
		body->activate(true);
		body->applyCentralForce(nConvert::ToBullet(accel));
	}
	void cBulletRigidBody::GetOrientation(glm::quat & orientation)
	{
		//orientation = this->orientation;
		orientation = nConvert::ToGlm(this->body->getOrientation());
	}
	void cBulletRigidBody::GetColliding(bool & colliding)
	{
		colliding = this->colliding;
	}

	void cBulletRigidBody::SetColliding(bool colliding)
	{
		if (colliding == true)
		{
			collidingCounter = 4.0f;
			this->colliding = true;
		}
		else
		{
			collidingCounter -= 0.1f;
			if (collidingCounter < 0)
			{
				this->colliding = false;
				collidingCounter = 0.0f;
			}
		}
		
	}

	void cBulletRigidBody::GetRayCasted(bool & rayCastedOut)
	{
		rayCastedOut = this->rayCasted;
	}

	void cBulletRigidBody::SetRayCasted(bool rayCastedIn)
	{
		this->rayCasted = rayCastedIn;
	}

	void cBulletRigidBody::GetRayCastPos(glm::vec3 & positionOut)
	{
		positionOut = lastRayCastPos;
	}

	void cBulletRigidBody::SetRayCastPos(glm::vec3 positionIn)
	{
		lastRayCastPos = positionIn;
	}

	void cBulletRigidBody::ZeroOutRotationalVel()
	{
		btVector3 zero = btVector3(0, 0, 0);
		body->setAngularVelocity(zero);
	}

	void cBulletRigidBody::GetRotationalVel(glm::vec3 & velocityOut)
	{
		velocityOut = nConvert::ToGlm(this->body->getAngularVelocity());
	}

	void cBulletRigidBody::SetRotationalVel(glm::vec3 & velocityIn)
	{
		body->activate(true);
		this->body->setAngularVelocity(nConvert::ToBullet(velocityIn));
	}

	void cBulletRigidBody::GetTransform(glm::mat4& transformOut)
	{
		nConvert::ToGlm(body->getWorldTransform(), transformOut);
	}
}