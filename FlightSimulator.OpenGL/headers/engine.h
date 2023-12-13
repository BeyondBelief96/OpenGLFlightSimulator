#ifndef ENGINE_H
#define ENGINE_H

#include "physics.h"
#include "rigidbody.h"

struct Engine
{
	float throttle = 1.0f, thrust;

	Engine() { thrust = 0.0f; }

	Engine(float thrust) : thrust(thrust) {};

	void apply_force(RigidBody* rigidBody)
	{
		// thrust is applied to the center of gravity and does not produce torque
		rigidBody->add_relative_force(physics::FORWARD * (throttle * thrust));
	}
};

#endif
