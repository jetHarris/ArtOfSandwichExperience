#pragma once


#include "btBulletDynamicsCommon.h"
#include <glm\game_math.h>

namespace nConvert
{
	inline btVector3 ToBullet(const glm::vec3& v)
	{
		return btVector3(v.x, v.y, v.z);
	}

	inline glm::vec3 ToGlm(const btVector3& v)
	{
		return glm::vec3(v.x(),v.y(),v.z());
	}

	inline void ToGlm(const btVector3& vIn, glm::vec3& vOut)
	{
		vOut.x = vIn.x();
		vOut.y = vIn.y();
		vOut.z = vIn.z();
	}


	inline void ToBullet(const  glm::vec3& vIn, btVector3& vOut)
	{
		vOut.setX(vIn.x);
		vOut.setY(vIn.y);
		vOut.setZ(vIn.z);
	}

	inline btQuaternion ToBullet(const glm::quat& q)
	{
		return btQuaternion(q.x, q.y, q.z, q.w);
	}

	inline glm::quat ToGlm(const btQuaternion& q)
	{
		return glm::quat(q.w(), q.x(), q.y(), q.z());
	}

	//MAT4
	inline void ToGlm(const btTransform& transform, glm::mat4& out)
	{
		transform.getOpenGLMatrix(&out[0][0]);
	}

	inline btTransform ToBullet(const glm::mat4& mat)
	{
		btTransform transform;
		transform.setFromOpenGLMatrix(&mat[0][0]);
		return transform;
	}
}
