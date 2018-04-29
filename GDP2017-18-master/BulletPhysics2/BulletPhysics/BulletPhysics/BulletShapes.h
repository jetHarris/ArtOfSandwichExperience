#pragma once
#include <glm\game_math.h>
#include <iShape.h>
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

namespace nPhysics
{
	class cBulletSphereShape : public iShape
	{
	public:
		cBulletSphereShape(float radius);

		virtual ~cBulletSphereShape();

		virtual bool GetSphereRadius(float& radiusOut);
		btCollisionShape* bulletShape;
	private:
		cBulletSphereShape();
		cBulletSphereShape(const cBulletSphereShape& other);
		cBulletSphereShape& operator=(const cBulletSphereShape& other);
		virtual void AddPoint(glm::vec3& inPoint) {};

		float radius;

	};

	class cBulletPlaneShape : public iShape
	{
	public:
		cBulletPlaneShape(const glm::vec3& normal, float planeConst, glm::vec3& pointOnPlane);
		cBulletPlaneShape(const glm::vec3& normal, float planeConst);
		virtual ~cBulletPlaneShape();

		virtual bool GetPlaneNormal(glm::vec3& normalOut);
		virtual bool GetPlaneConst(float& planeConstOut);
		btCollisionShape* bulletShape;
		glm::vec3 normal;
		float planeConst;
	private:
		cBulletPlaneShape();
		cBulletPlaneShape(const cBulletPlaneShape& other);
		cBulletPlaneShape& operator=(const cBulletPlaneShape& other);
		virtual void AddPoint(glm::vec3& inPoint) {};

		
	};

	class cBulletBoxShape : public iShape
	{
	public:
		cBulletBoxShape(glm::vec3 boxHalfWidths);
		virtual ~cBulletBoxShape();
		btCollisionShape* bulletShape;
	private:
		cBulletBoxShape();
		cBulletBoxShape(const cBulletBoxShape& other);
		cBulletBoxShape& operator=(const cBulletBoxShape& other);
		virtual void AddPoint(glm::vec3& inPoint) {};
	};

	class cBulletCylinderShape : public iShape
	{
	public:
		cBulletCylinderShape(glm::vec3 boxHalfWidths);
		virtual ~cBulletCylinderShape();
		btCollisionShape* bulletShape;
	private:
		cBulletCylinderShape();
		cBulletCylinderShape(const cBulletCylinderShape& tempCylinder);
		cBulletCylinderShape& operator=(const cBulletCylinderShape& other);
		virtual void AddPoint(glm::vec3& inPoint) {};
	};

	class cBulletConeShape : public iShape
	{
	public:
		cBulletConeShape(float height, float radius);
		virtual ~cBulletConeShape();
		btCollisionShape* bulletShape;
	private:
		cBulletConeShape();
		cBulletConeShape(const cBulletConeShape& tempCylinder);
		cBulletConeShape& operator=(const cBulletConeShape& other);
		virtual void AddPoint(glm::vec3& inPoint) {};
	};

	class cBulletConvexHullShape : public iShape
	{
	public:
		cBulletConvexHullShape(float * points, int numPoints, int stride);
		cBulletConvexHullShape(float * points, int numPoints);
		virtual ~cBulletConvexHullShape();
		btCollisionShape* bulletShape;
		virtual void AddPoint(glm::vec3& inPoint);
		cBulletConvexHullShape();
	private:
		cBulletConvexHullShape(const cBulletConeShape& tempHull);
		cBulletConvexHullShape& operator=(const cBulletConvexHullShape& other);
	};
}