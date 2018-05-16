#ifndef _cEmitter_HG_
#define _cEmitter_HG_

#include <vector>
#include "cParticle.h"
#include <glm\vec4.hpp>

class cEmitter
{
public:
	cEmitter();
	cEmitter(glm::vec3 pos);
	~cEmitter();
	void init(int numParticles, int maxParticleCreatedPerStep,
		glm::vec3 minInitVel, glm::vec3 maxInitVel,
		float minLife, float maxLife);
	glm::vec3 position;
	void Update(float deltaTime);
	bool emitting = false;

	std::vector< cParticle* > m_vecParticles;
	std::vector< glm::vec4> particleColours;
	int partColSize;
	float fireCurrentCenterX;
	float fireCenterResetCounter;
	float xRange;
private:
	int m_numParticles;
	int m_maxParticleCreatedPerStep;
	glm::vec3 m_minInitVel;
	glm::vec3 m_maxInitVel;
	float m_minLife;
	float m_maxLife;
	glm::vec3 m_minRangeFromEmitter;
	glm::vec3 m_maxRangeFromEmitter;
	glm::vec3 m_acceleration;

};

#endif 

