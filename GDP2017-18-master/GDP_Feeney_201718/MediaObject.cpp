#include "MediaObject.h"

//constructor
MediaObject::MediaObject()
{
	streamObject = false;
	fileName = "";
	pan = 0.0f;
	frequency = 44100;
	pitch = 1.0f;
	volume = 1.0f;
	playBackSpeed = 1.0f;
	groupID = 0;
	ThirdDimension = false;
	position = glm::vec3(0.0f);
}

//destructor
MediaObject::~MediaObject()
{

}

void MediaObject::Release()
{
	sound->release();
	//delete channel;
}
