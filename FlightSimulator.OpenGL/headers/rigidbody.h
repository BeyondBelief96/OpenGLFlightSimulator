#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class RigidBody
{
private:
	glm::vec3 m_force{};  // world space
	glm::vec3 m_torque{}; // body space
public:
	float mass = 1.0f; // kg
	glm::vec3 position{}; // world space, meters
	glm::quat orientation{}; // world space.
	glm::vec3 velocity{};    // world space, meters/second
	glm::vec3 angular_velocity{}; // body space, radians/second
	glm::mat3 inertia{}, inverse_inertia{}; // inertia tensor, body space.
	bool apply_gravity = true;

	RigidBody(float mass, glm::mat3 inertia) : mass(mass), inertia(inertia) {};

	//transform direction from body space to world space
	inline glm::vec3 transform_body_to_world(const glm::vec3 direction) const
	{
		return orientation * direction;
	}

	// transform direction from world space to body space
	inline glm::vec3 transform_world_to_body(const glm::vec3& direction) const
	{
		return glm::inverse(orientation) * direction;
	}

	// get velocity and angular velocity in body space
	inline glm::vec3 get_point_velocity(const glm::vec3& point) const
	{
		return transform_world_to_body(velocity) + glm::cross(angular_velocity, point);
	}

	// force and point vectors are in body space
	inline void add_force_at_point(const glm::vec3& force, const glm::vec3& point)
	{
		m_force += transform_body_to_world(force), m_torque += glm::cross(point, force);
	}

	// force vector in body space
	inline void add_relative_force(const glm::vec3& force)
	{
		m_force += transform_body_to_world(force);
	}

	// integrate using the euler method
	virtual void update(float dt)
	{
		// integrate position
		glm::vec3 acceleration = m_force / mass;
		if (apply_gravity) acceleration.y -= 9.81f;
		velocity += acceleration * dt;
		position += velocity * dt;

		// Integrate orientation
		// Update the angular velocity
		angular_velocity += inverse_inertia *
			(m_torque - glm::cross(angular_velocity, inertia * angular_velocity)) * dt;
		/*
		angular_velocity: The current angular velocity of the rigid body.
		inverse_inertia: The inverse of the inertia tensor, indicating how mass is distributed.
		m_torque: The external torque applied to the rigid body.
		glm::cross(angular_velocity, inertia * angular_velocity): Calculates the gyroscopic effect.
		This is the cross product of angular velocity and the inertia-scaled angular velocity,
		accounting for how a rotating body resists changes in its axis of rotation.
		dt: The small time step over which the integration is performed.
		The entire expression computes the change in angular velocity considering both
		the applied torque and the body's resistance to change in rotation (gyroscopic effects).
		*/

		// Update the orientation
		orientation += (orientation * glm::quat(0.0f, angular_velocity)) * (0.5f * dt);
		/*
		orientation: The quaternion representing the rigid body's current orientation.
		glm::quat(0.0f, angular_velocity): Converts angular velocity to a quaternion.
		This step is crucial for quaternion-based rotation updates.
		The multiplication of orientation with this quaternion represents the change in
		orientation due to the angular velocity over the time step.
		0.5f * dt: Scales the rotation increment. The factor 0.5 is often used for
		numerical stability and better accuracy in integration.
		The updated orientation is the sum of the current orientation and the calculated increment.
		*/

		// Normalize the orientation quaternion
		orientation = glm::normalize(orientation);
		/*
		Normalizing the orientation quaternion is essential after the update.
		Quaternion operations can lead to a quaternion that is not a unit quaternion.
		Normalizing ensures that the orientation remains valid and represents a proper
		rotation without any scaling effects.
		*/


		// reset accumulators
		m_force = glm::vec3(0.0f), m_torque = glm::vec3(0.0f);
	}
};

#endif