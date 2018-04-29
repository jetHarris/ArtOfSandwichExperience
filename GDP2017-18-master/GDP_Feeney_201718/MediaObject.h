#ifndef _MEDIAOBJECT_HG_
#define _MEDIAOBJECT_HG_

#include "../GDP_Feeney_201718/include/fmod/fmod.hpp"
#include "../GDP_Feeney_201718/include/fmod/fmod_errors.h"
#include <string>
#include <glm/vec3.hpp>

class MediaObject
{
public:
	//constructor
	MediaObject();
	//destructor
	~MediaObject();
	void Release();
	//member variables
	FMOD::Sound* sound;
	FMOD::Channel* channel;
	bool streamObject;
	std::string fileName;
	float pan;
	float volume;
	float frequency;
	float pitch;
	float playBackSpeed;
	int groupID;
	bool ThirdDimension;
	glm::vec3 position;
};


#endif // !_MEDIAOBJECT_HG_
