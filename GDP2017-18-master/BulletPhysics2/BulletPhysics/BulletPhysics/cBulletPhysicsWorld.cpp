#include "cBulletPhysicsWorld.h"
#include <algorithm>
#include <iostream>
#include "BulletShapes.h"
#include "BulletConstraints.h"
#include "nConvert.h"


namespace nPhysics
{
	//dtor
	cBulletPhysicsWorld::~cBulletPhysicsWorld()
	{
		for (int i = 0; i != this->rigidBodies.size(); i++)
		{
			//delete this->rigidBodies[i]->shape;
			delete this->rigidBodies[i];
		}

		delete this->dynamicsWorld;
		delete this->solver; 
		delete this->overlappingPairCache;
		delete this->dispatcher;
		delete this->collisionConfiguration;
		//delete this->broadPhase;
		
	}

	cBulletPhysicsWorld::cBulletPhysicsWorld()
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		dispatcher = new btCollisionDispatcher(collisionConfiguration);

		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		overlappingPairCache = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		solver = new btSequentialImpulseConstraintSolver;

		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(0, -20, 0));
	}

	cBulletPhysicsWorld::cBulletPhysicsWorld(btVector3 gravity)
	{
		btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

		btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		dynamicsWorld->setGravity(gravity);
	}

	//not in hello world example
	cBulletPhysicsWorld::cBulletPhysicsWorld(glm::vec3 max, glm::vec3 min)
	{
	}


	void cBulletPhysicsWorld::TimeStep(float deltaTime)
	{
		this->dynamicsWorld->stepSimulation(deltaTime);
		//maybe go through all the rigid bodies and set colliding to false before the collision step
		float rigidBodiesSize = rigidBodies.size();
		for (int i = 0; i < rigidBodiesSize; ++i)
		{
			if (rigidBodies[i] != NULL)
			{
				rigidBodies[i]->SetColliding(false);
			}
		}
		Collisions();
	}

	void cBulletPhysicsWorld::Collisions()
	{
		int numManifolds = this->dynamicsWorld->getDispatcher()->getNumManifolds();
		for (int index = 0; index != numManifolds; index++)
		{
			btPersistentManifold* contactManifold = this->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(index);

			if (contactManifold)
			{
				if (contactManifold->getNumContacts() > 0)
				{ 

					if (contactManifold->getContactPoint(0).getLifeTime() <= 5) // this collision point has just been created
					{

						const btCollisionObject* obA = contactManifold->getBody0();
						const btCollisionObject* obB = contactManifold->getBody1();

						iRigidBody* bodyA = reinterpret_cast<iRigidBody*>(obA->getUserPointer());
						iRigidBody* bodyB = reinterpret_cast<iRigidBody*>(obB->getUserPointer());

						if (bodyA && bodyB)
						{
							//set colliding to true for the rigid bodies
							bodyA->SetColliding(true);
							bodyB->SetColliding(true);
						}
					}
				}
			}
		}
	}

	//add a rigid body to the scene
	void cBulletPhysicsWorld::AddRigidBody(iRigidBody* rigidBody)
	{

		cBulletRigidBody* myBody = dynamic_cast<cBulletRigidBody*>(rigidBody);
		if (!myBody)
		{
			return; //not good :(
		}
		btRigidBody* btBody = myBody->body;
		if (btBody->isInWorld())
		{
			return; //already in the world
		}
		dynamicsWorld->addRigidBody(btBody);
		rigidBodies.push_back(rigidBody);
		return;

	}

	//remove a rigid body from the scene
	void cBulletPhysicsWorld::RemoveRigidBody(iRigidBody* rigidBody)
	{
		cBulletRigidBody* myBody = dynamic_cast<cBulletRigidBody*>(rigidBody);
		if (myBody == NULL)
		{
			return; //not good :(
		}
		btRigidBody* btBody = myBody->body;
		//if (!btBody->isInWorld())
		//{
		//	return; //not in world already
		//}
		dynamicsWorld->removeRigidBody(btBody);

		//also go remove it from the rigid bodies vector
		int index = -1;
		int bodiesSize = rigidBodies.size();
		for (int i = 0; i < bodiesSize; ++i)
		{
			if (rigidBodies[i] == rigidBody)
			{
				index = i;
				break;
			}
		}

		if (index != -1)
		{
			rigidBodies.erase(rigidBodies.begin() + index);
			delete rigidBody;
		}

		return;
	}


	void cBulletPhysicsWorld::SetDebugRenderer(iDebugRenderer * debugRenderer)
	{
	}

	void cBulletPhysicsWorld::RenderDebug()
	{
	}

	//to test for and handle a collision between 2 rigid bodies
	void cBulletPhysicsWorld::Collide(iRigidBody * first, iRigidBody * second)
	{
		eShapeType typeA = first->GetShape()->GetShapeType();
		eShapeType typeB = second->GetShape()->GetShapeType();
		switch (typeA)
		{
		case eShapeType::SHAPE_TYPE_SPHERE: //if the first shape is a sphere
		{

			cBulletSphereShape* sphereA = dynamic_cast<cBulletSphereShape*>(first->GetShape());
			switch (typeB)
			{
			case eShapeType::SHAPE_TYPE_SPHERE:
			{
				//do sphere sphere collision
				cBulletSphereShape* sphereB = dynamic_cast<cBulletSphereShape*>(second->GetShape());

				//call a more specific method to test for and handle the collision
				return CollideSphereSphere(first, second);
			}
			break;
			case eShapeType::SHAPE_TYPE_PLANE:
			{
				//do sphere plane collision
				cBulletPlaneShape* planeB = dynamic_cast<cBulletPlaneShape*>(second->GetShape());

				//call a more specific method to test for and handle the collision
				return CollideSpherePlane(first, second);
			}
			break;
			}
		}
		break;
		case eShapeType::SHAPE_TYPE_PLANE: //if the first shape is a plane
		{
			cBulletPlaneShape* planeA = dynamic_cast<cBulletPlaneShape*>(first->GetShape());
			switch (typeB)
			{
			case eShapeType::SHAPE_TYPE_SPHERE:
			{
				//do plane sphere collision
				cBulletSphereShape* sphereB = dynamic_cast<cBulletSphereShape*>(second->GetShape());

				//call a more specific method to test for and handle the collision
				return CollideSpherePlane(second, first);
			}
			break;
			}
		}
		break;
		}
	}

	void cBulletPhysicsWorld::CollideSphereSphere(iRigidBody * first, iRigidBody * second)
	{
	}

	void cBulletPhysicsWorld::CollideSpherePlane(iRigidBody * first, iRigidBody * second)
	{
	}

	bool cBulletPhysicsWorld::AddConstraint(iConstraint* constraint)
	{
		if (!constraint) return false;
		cBulletConstraint* bulletConstraint = dynamic_cast<cBulletConstraint*>(constraint);
		if (!bulletConstraint) return false;
		if (bulletConstraint->constraint->getUserConstraintPtr()) return false; // already in the world
		bulletConstraint->constraint->setUserConstraintPtr(bulletConstraint);
		this->dynamicsWorld->addConstraint(bulletConstraint->constraint);
		return true;
	} // end AddConstraint()
	bool cBulletPhysicsWorld::RemoveConstraint(iConstraint* constraint)
	{
		if (!constraint) return false;
		cBulletConstraint* bulletConstraint = dynamic_cast<cBulletConstraint*>(constraint);
		if (!bulletConstraint) return false;
		if (!bulletConstraint->constraint->getUserConstraintPtr()) return false; // not in the world
		bulletConstraint->constraint->setUserConstraintPtr(0);
		this->dynamicsWorld->removeConstraint(bulletConstraint->constraint);
		return true;
	} // end RemoveConstraint()


	void cBulletPhysicsWorld::RayCast(glm::vec3 start, glm::vec3 end)
	{
		btDynamicsWorld::ClosestRayResultCallback RayCallback(nConvert::ToBullet(start), nConvert::ToBullet(end));

		dynamicsWorld->rayTest(nConvert::ToBullet(start), nConvert::ToBullet(end), RayCallback);
		float rigidBodiesSize = rigidBodies.size();
		for (int i = 0; i < rigidBodiesSize; ++i)
		{
			if (rigidBodies[i] != NULL)
			{
				rigidBodies[i]->SetRayCasted(false);
			}
		}

		if (RayCallback.hasHit()) {
			((iRigidBody*)RayCallback.m_collisionObject->getUserPointer())->SetRayCastPos(nConvert::ToGlm(RayCallback.m_hitPointWorld));
			((iRigidBody*)RayCallback.m_collisionObject->getUserPointer())->SetRayCasted(true);
		}

	}
}