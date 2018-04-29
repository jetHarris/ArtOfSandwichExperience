#include "BulletShapes.h"

namespace nPhysics
{
	inline btVector3 ToBullet(const glm::vec3& inVec) {
		return btVector3(inVec.x, inVec.y, inVec.z);
	}

	cBulletSphereShape::cBulletSphereShape(float radius)
		: iShape(SHAPE_TYPE_SPHERE)
		, radius(radius)
	{
		bulletShape = new btSphereShape(radius);
	}
	cBulletSphereShape::cBulletSphereShape()
		: iShape(SHAPE_TYPE_SPHERE)
	{

	}

	cBulletSphereShape::cBulletSphereShape(const cBulletSphereShape& other)
		: iShape(SHAPE_TYPE_SPHERE)
	{

	}
	cBulletSphereShape& cBulletSphereShape::operator=(const cBulletSphereShape& other)
	{
		return *this;
	}
	cBulletSphereShape::~cBulletSphereShape()
	{

	}
	bool cBulletSphereShape::GetSphereRadius(float& radiusOut)
	{
		radiusOut = radius;
		return true;
	}
	cBulletPlaneShape::cBulletPlaneShape(const glm::vec3& normal, float planeConst, glm::vec3& pointOnPlane)
		: iShape(SHAPE_TYPE_PLANE)
		, normal(normal)
		, planeConst(planeConst)
	{
		//bulletShape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z),glm::dot(normal, pointOnPlane));
	}
	cBulletPlaneShape::cBulletPlaneShape(const glm::vec3 & normal, float planeConst)
		: iShape(SHAPE_TYPE_PLANE)
		, normal(normal)
		, planeConst(planeConst)
	{
	}
	cBulletPlaneShape::cBulletPlaneShape()
		: iShape(SHAPE_TYPE_PLANE)
	{
	}

	cBulletPlaneShape::cBulletPlaneShape(const cBulletPlaneShape& other)
		: iShape(SHAPE_TYPE_PLANE)
	{

	}
	cBulletPlaneShape& cBulletPlaneShape::operator=(const cBulletPlaneShape& other)
	{
		return *this;
	}
	cBulletPlaneShape::~cBulletPlaneShape()
	{

	}
	bool cBulletPlaneShape::GetPlaneNormal(glm::vec3& normalOut)
	{
		normalOut = normal;
		return true;
	}
	bool cBulletPlaneShape::GetPlaneConst(float& planeConstOut)
	{
		planeConstOut = planeConst;
		return true;
	}

	cBulletBoxShape::cBulletBoxShape(glm::vec3 boxHalfWidths) : iShape(SHAPE_TYPE_BOX)
	{
		bulletShape = new btBoxShape(ToBullet(boxHalfWidths));
	}
	cBulletBoxShape::~cBulletBoxShape()
	{
	}
	cBulletCylinderShape::cBulletCylinderShape(glm::vec3 boxHalfWidths)
	{
		//the commented out ones are options for aligning the cylinder on a different action
		//bulletShape = new btCylinderShape(ToBullet(boxHalfWidths));
		//bulletShape = new btCylinderShapeX(ToBullet(boxHalfWidths));
		bulletShape = new btCylinderShapeZ(ToBullet(boxHalfWidths));
	}
	cBulletCylinderShape::~cBulletCylinderShape()
	{
	}
	cBulletConeShape::cBulletConeShape(float height, float radius)
	{
		bulletShape = new btConeShape(height, radius);
	}
	cBulletConeShape::~cBulletConeShape()
	{
	}
	cBulletConvexHullShape::cBulletConvexHullShape(float * points, int numPoints, int stride)
	{
		bulletShape = new btConvexHullShape(points,numPoints,stride);
	}
	cBulletConvexHullShape::cBulletConvexHullShape(float * points, int numPoints)
	{
		bulletShape = new btConvexHullShape(points, numPoints);
	}
	cBulletConvexHullShape::~cBulletConvexHullShape()
	{
	}
	void cBulletConvexHullShape::AddPoint(glm::vec3 & inPoint)
	{
		((btConvexHullShape*)bulletShape)->addPoint(ToBullet(inPoint));
	}
	cBulletConvexHullShape::cBulletConvexHullShape()
	{
		bulletShape = new btConvexHullShape();
	}
}