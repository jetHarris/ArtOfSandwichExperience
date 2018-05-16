#include "cEmitter.h"
#include "cCamera.h"
#include "cGameObject.h"
#include <iostream>

extern cGameObject* pToaster;



cEmitter::cEmitter()
{
	this->m_numParticles = 0;
	this->m_maxParticleCreatedPerStep = 0;
	this->m_minLife = 0.0f;
	this->m_maxLife = 0.0f; 
	this->m_maxInitVel = glm::vec3(0.0f);
	this->m_minInitVel = glm::vec3(0.0f);
	this->m_minRangeFromEmitter = glm::vec3(0.0f);
	this->m_maxRangeFromEmitter = glm::vec3(0.0f);	
	return;
}

cEmitter::cEmitter(glm::vec3 pos)
{
	this->m_numParticles = 0;
	this->m_maxParticleCreatedPerStep = 0;
	this->m_minLife = 0.0f;
	this->m_maxLife = 0.0f;
	this->m_maxInitVel = glm::vec3(0.0f);
	this->m_minInitVel = glm::vec3(0.0f);
	this->m_minRangeFromEmitter = glm::vec3(0.0f);
	this->m_maxRangeFromEmitter = glm::vec3(0.0f);
	this->xRange = 1.8;

	position = pos;
	
	//the particles for the emitter will transition through these colours based on lifetime
	particleColours.push_back(glm::vec4(0, 0, 0, 0.2));
	particleColours.push_back(glm::vec4(0, 0, 0, 0.5));
	particleColours.push_back(glm::vec4(237.0 / 255.0, 0, 0, 1));
	particleColours.push_back(glm::vec4(237.0 / 255.0, 97.0 / 255.0, 16.0 / 255.0, 1));
	particleColours.push_back(glm::vec4(237.0 / 255.0, 97.0 / 255.0, 16.0 / 255.0, 1));
	particleColours.push_back(glm::vec4(244.0/255.0, 244.0 / 255.0, 66.0 / 255.0, 1));

	partColSize = particleColours.size();
	fireCurrentCenterX = 0;
	fireCenterResetCounter = 1;
}

cEmitter::~cEmitter()
{
	// Delete the particles
	return;
}

double getRandInRange(double min, double max)
{

	double value = min + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (max - min)));
	return value;
}

// Call this only to start of if you want to change the initial values
// (NOT to be called every frame or anything)
// (Also, call IN ADVANCE of "expensive" operations, like explosions, 
//  or other things that require a lot of particles)
void cEmitter::init(
	int numParticles, int maxParticleCreatedPerStep,
	glm::vec3 minInitVel, glm::vec3 maxInitVel,
	float minLife, float maxLife)
{
	// Do I already have this many particles in my vector? 
	int numParticlesToCreate = numParticles - this->m_vecParticles.size();
	// If we DON'T have enough, then create some
	for (int count = 0; count <= numParticlesToCreate; count++)
	{	// 
		this->m_vecParticles.push_back(new cParticle());
		this->m_vecParticles.back()->pos = position;
	}
	// At this point, we have enough particles

	// Reset all the particles to the default position and life
	this->m_numParticles = numParticles;
	for (int index = 0; index != this->m_numParticles; index++)
	{	// reset ONLY the lifetime
		this->m_vecParticles[index]->lifetime = 0.0f;
	}

	// Save all the init values we passed
	this->m_maxParticleCreatedPerStep = maxParticleCreatedPerStep;
	this->m_minInitVel = minInitVel;
	this->m_maxInitVel = maxInitVel;
	this->m_minLife = minLife;
	this->m_maxLife = maxLife;
	return;
}


void cEmitter::Update(float deltaTime)
{

	glm::quat currentToasterOrienation  = pToaster->getQOrientation();

	int particlesToStillMake = this->m_maxParticleCreatedPerStep;

	fireCenterResetCounter -= deltaTime;

	glm::vec3 tempToasterPos = pToaster->getPosition();

	if (fireCenterResetCounter < 0)
	{
		fireCenterResetCounter = getRandInRange(0.1, 1);
		fireCurrentCenterX = getRandInRange(-xRange, xRange);
		fireCurrentCenterX += tempToasterPos.x;
	}

	// Scan through the vector, looking for "dead" particles
	if (emitting) {
		for (int index = 0;
			(index != this->m_numParticles) && (particlesToStillMake > 0);
			index++)
		{
			// Is this particle 'dead'
			if (this->m_vecParticles[index]->lifetime <= 0.0f)
			{	// "create" a particle
				// Assign values to particles

				
				this->m_vecParticles[index]->colour = glm::vec4(1, 1, 1, 1);
				glm::vec4 newPos = glm::vec4(0);
				newPos.x = getRandInRange(-xRange, xRange);
				newPos.z = getRandInRange(-0.4, 0.4);
				newPos.y = position.y;

				//newPos = newPos * currentToasterOrienation;

				float distToCenter = abs(newPos.x - fireCurrentCenterX);


				this->m_vecParticles[index]->lifetime =
					getRandInRange(this->m_minLife, this->m_maxLife);
				this->m_vecParticles[index]->lifetime += distToCenter * 0.1;

				int sparkChance = rand() % 80;
				if (sparkChance == 1)
				{
					this->m_vecParticles[index]->lifetime *= 3;
				}
				this->m_vecParticles[index]->startingLifeTime = this->m_vecParticles[index]->lifetime;

				
				
				newPos.x += tempToasterPos.x;
				newPos.y += tempToasterPos.y;
				newPos.z += tempToasterPos.z;

				this->m_vecParticles[index]->pos = glm::vec3(newPos.x, newPos.y, newPos.z);

				this->m_vecParticles[index]->vel.x = m_minInitVel.x;
					//getRandInRange(this->m_minInitVel.x, this->m_maxInitVel.x);
				this->m_vecParticles[index]->vel.y =
					getRandInRange(this->m_minInitVel.y, this->m_maxInitVel.y);
				this->m_vecParticles[index]->vel.y += distToCenter * 3;
				



				this->m_vecParticles[index]->vel.z = this->m_minInitVel.z;
					//getRandInRange(this->m_minInitVel.z, this->m_maxInitVel.z);

				// Update the count
				particlesToStillMake--;
			}
		}//for (int index
	}

	 // 4. Perform integration step (aka Euler, or use physics, etc.)
	 //    (ALSO: decrease the 'life' of the particle by deltaTime)
	for (int index = 0; index != this->m_numParticles; index++)
		//	for (int index = 0; index != this->m_vecParticles.size()-1; index++)
	{
		// Forward explicit Euler...		
		// Velocity comes from accleration 
		//this->m_vecParticles[index]->vel += (this->m_acceleration * deltaTime);

		// Position comes from velocity
		this->m_vecParticles[index]->pos +=
			(this->m_vecParticles[index]->vel * deltaTime);

		// Update the lifetime
		this->m_vecParticles[index]->lifetime -= deltaTime;

		if (this->m_vecParticles[index]->lifetime > 0)
		{
			float ratio = this->m_vecParticles[index]->lifetime /
				this->m_vecParticles[index]->startingLifeTime;

			unsigned int colID = ratio * partColSize;
			//colID--;
			//std::cout << "ColID: " << colID << std::endl;
			this->m_vecParticles[index]->colour = particleColours[colID];

			//if (this->m_vecParticles[index]->lifetime < this->m_vecParticles[index]->startingLifeTime / 2.0f)
			//{
			//	this->m_vecParticles[index]->colour = glm::vec4(1, 0, 0, 1);
			//}
			//else
			//{

			//}
		}


	}

	return;
}

