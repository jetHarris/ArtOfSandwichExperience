#ifndef cINTEGRATOR_HG
#define cINTEGRATOR_HG
#include <glm\vec3.hpp>

struct State
{
	State() {}
	State(glm::vec3 pos, glm::vec3 vel, glm::vec3 accel) : position(pos), velocity(vel), acceleration(accel) {}
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	
};

struct StateDerivative
{
	glm::vec3 positionDerv;
	glm::vec3 velocityDerv;
};

class cBulletIntegrator
{
public:

	cBulletIntegrator();
	~cBulletIntegrator();

	glm::vec3 acceleration(const State & state, double t);

	StateDerivative evaluate(const State & initial, double t, float dt, const StateDerivative & d);
	StateDerivative evaluate(const State & initial, float dt, const StateDerivative & d);

	void integrate(State & state, double t, float dt);
	void integrate(State & state, float dt);
};



#endif // !cINTEGRATOR_HG

