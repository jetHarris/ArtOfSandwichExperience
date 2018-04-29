#include "cCamera.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "cGameObject.h"
#include <iostream>


cCamera::cCamera()
{
	this->cameraPos = glm::vec3(30.0f, 20.0f, 13.0f);
	this->cameraFront = glm::vec3(-1.0f, 0.0f, 0.0f);
	this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	return;
}

cCamera::~cCamera()
{
	return;
}

glm::mat4 cCamera::getViewMatrix(void)
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::vec3 cCamera::getEyePosition()
{
	return cameraPos;
}


