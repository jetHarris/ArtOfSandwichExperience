#pragma once
#include <glm\game_math.h>

namespace nPhysics
{
	class iDebugRenderer
	{
	public:
		virtual ~iDebugRenderer() {}
		//virtual void Clear() = 0;
		virtual void RenderSphere(const glm::mat4& transform, const glm::vec4& colour) = 0;
		virtual void RenderPlane(const glm::vec3& normal, float planeConst, const glm::vec4& colour) = 0;
	};
}