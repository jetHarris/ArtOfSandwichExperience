#ifndef _cParticle_HG_
#define _cParticle_HG_

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class cParticle
{
public:
	cParticle()
	{
		lifetime = 0.0f;
		pos = glm::vec3(0.0f);
		vel = glm::vec3(0.0f);
		colour = glm::vec4(1, 1, 1, 1);
	};
	glm::vec3 pos;
	glm::vec3 vel;
	glm::vec4 colour;
	float lifetime;
	float startingLifeTime;
	// And other things (colour, whatever)
};

#endif
