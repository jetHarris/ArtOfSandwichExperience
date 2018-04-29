#include "cBulletIntegrator.h"

cBulletIntegrator::cBulletIntegrator()
{
}

cBulletIntegrator::~cBulletIntegrator()
{
}

glm::vec3 cBulletIntegrator::acceleration(const State & state, double t)
{
	return glm::vec3(0, -5, 0) +state.acceleration;
}

StateDerivative cBulletIntegrator::evaluate(const State & initial, double t, float dt, const StateDerivative & d)
{
	State state;
	state.position = initial.position + (d.positionDerv *dt);
	state.velocity = initial.velocity + (d.velocityDerv * dt);
	state.acceleration = initial.acceleration;

	StateDerivative output;
	output.positionDerv = state.velocity;
	output.velocityDerv = acceleration(state, t + dt);
	return output;
}

StateDerivative cBulletIntegrator::evaluate(const State & initial, float dt, const StateDerivative & d)
{
	State state;
	state.position = initial.position + (d.positionDerv *dt);
	state.velocity = initial.velocity + (d.velocityDerv * dt);
	state.acceleration = initial.acceleration;

	StateDerivative output;
	output.positionDerv = state.velocity;
	output.velocityDerv = acceleration(state, dt);
	return output;
}

void cBulletIntegrator::integrate(State & state, double t, float dt)
{
	StateDerivative a, b, c, d;

	a = evaluate(state, t, 0.0f, StateDerivative());
	b = evaluate(state, t, dt* 0.5f, a);
	c = evaluate(state, t, dt* 0.5f, b);
	d = evaluate(state, t, dt, c);

	glm::vec3 dxdt = (1.0f / 6.0f) * (a.positionDerv + 2.0f * (b.positionDerv + c.positionDerv) + d.positionDerv);
	glm::vec3 dvdt = (1.0f / 6.0f) * (a.velocityDerv + 2.0f * (b.velocityDerv + c.velocityDerv) + d.velocityDerv);

	state.position = state.position + dxdt * dt;
	state.velocity = state.velocity + dvdt * dt;
}

void cBulletIntegrator::integrate(State & state, float dt)
{
	StateDerivative a, b, c, d;

	a = evaluate(state, 0.0f, StateDerivative());
	b = evaluate(state, dt* 0.5f, a);
	c = evaluate(state, dt* 0.5f, b);
	d = evaluate(state, dt, c);

	glm::vec3 dxdt = (1.0f / 6.0f) * (a.positionDerv + 2.0f * (b.positionDerv + c.positionDerv) + d.positionDerv);
	glm::vec3 dvdt = (1.0f / 6.0f) * (a.velocityDerv + 2.0f * (b.velocityDerv + c.velocityDerv) + d.velocityDerv);

	state.position = state.position + dxdt * dt;
	state.velocity = state.velocity + dvdt * dt;
}

