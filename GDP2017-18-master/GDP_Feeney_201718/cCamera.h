#ifndef _cCamera_HG_
#define _cCamera_HG_

#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <string>
class cGameObject;

class cCamera
{
public:
	cCamera();
	~cCamera();

	glm::mat4 getViewMatrix(void);
	glm::vec3 getEyePosition();
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
};

#endif
