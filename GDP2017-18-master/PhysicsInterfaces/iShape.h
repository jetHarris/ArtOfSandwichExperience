#pragma once
#include "eShapeType.h"
#include <glm\game_math.h>

namespace nPhysics
{
	class iShape
	{
	public:
		virtual ~iShape() {}

		eShapeType GetShapeType() { return mShapeType; }

		virtual bool GetSphereRadius(float& radiusOut) { return false; }
		virtual bool GetPlaneNormal(glm::vec3& normalOut) { return false; }
		virtual bool GetPlaneConst(float& planeConstOut) { return false; }
		virtual void AddPoint(glm::vec3& inPoint) = 0;

	protected:
		iShape(eShapeType shapeType)
			: mShapeType(shapeType)
		{

		}
		iShape() {}
		iShape(const iShape& other) {}
		iShape& operator=(const iShape& other) { return *this; }
	private:
		eShapeType mShapeType;
	};
}