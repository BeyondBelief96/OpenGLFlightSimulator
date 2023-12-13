#ifndef AIRPLANE_H
#define AIRPLANE_H

#include <vector>

#include "rigidbody.h"
#include "engine.h"
#include "wing.h"

class Airplane : public RigidBody
{
private:
	Engine engine;
	std::vector<Wing> airfoils;
public:
	
	// Default constructor
	Airplane() : RigidBody(0, glm::mat3())
	{
		engine = Engine();
		airfoils = std::vector<Wing>();
	}

	// wings are in the order { left_wing, right_wing, elevator, rudder }
	Airplane(float mass, float thrust, const glm::mat3& inertia, std::vector<Wing> wings) :
		RigidBody(mass, inertia), airfoils(wings), engine(thrust) {};

	void update(float dt) override
	{
		engine.apply_force(this);

		for (auto& wing : airfoils)
		{
			wing.apply_forces(this, dt);
		}

		RigidBody::update(dt);
	}
};

#endif