#ifndef WING_H
#define WING_H

#include <tuple>
#include <math.h>
#include <glm/glm.hpp>

#include "physics.h"
#include "rigidbody.h"

// AoA, Cl, Cd
using AeroData = glm::vec3;

// Define a structure to model the aerodynamic properties of an airfoil.
struct Airfoil {
    float min_alpha, max_alpha;  // Minimum and maximum angles of attack for the airfoil.
    float cl_max;                // Maximum lift coefficient of the airfoil.
    int max_index;               // Index of the last element in the aerodynamic data vector.
    std::vector<AeroData> data;  // Vector storing aerodynamic data for different angles of attack.

    // Constructor that initializes the airfoil with a given lift curve.
    Airfoil(const std::vector<AeroData>& curve) : data(curve)
    {
        // Initialize min_alpha and max_alpha based on the first and last elements of the curve.
        min_alpha = curve.front().x, max_alpha = curve.back().x;
        // Compute the index of the last data point.
        max_index = static_cast<int>(data.size() - 1);

        // Initialize cl_max to zero, then find the maximum lift coefficient in the curve.
        cl_max = 0.0f;
        for (auto& val : curve) {
            if (val.y > cl_max) cl_max = val.y;
        }
    }

    // Function to sample the lift and drag coefficients at a specific angle of attack.
    std::tuple<float, float> sample(float alpha) const
    {
        // Normalize alpha within the range [min_alpha, max_alpha] and scale by max_index.
        float t = physics::inverse_lerp(min_alpha, max_alpha, alpha) * max_index;
        // Separate the normalized value into its integer and fractional parts.
        float integer = std::floor(t);
        float fractional = t - integer;
        int index = static_cast<int>(integer);

        // Linearly interpolate between the data points if alpha is within the range,
        // otherwise use the last data point.
        auto value = (index < max_index) ? physics::lerp(data[index], data[index + 1], fractional) : data[max_index];

        // Return the lift and drag coefficients as a tuple.
        return { value.y, value.z };
    }
};

// wing element
struct Wing {
    const float area;
    const float wingspan;
    const float chord;
    const float aspect_ratio;
    const Airfoil* airfoil;
    const glm::vec3 normal;
    const glm::vec3 center_of_pressure;
    const float flap_ratio;  // percentage of wing that is part of the flap
    const float efficiency_factor = 1.0f;

    float control_input = 0.0f;

    // relative position of leading edge to cg
    Wing(const Airfoil* airfoil, const glm::vec3& relative_position, float area, float span, const glm::vec3& normal,
        float flap_ratio = 0.25f)
        : airfoil(airfoil),
        center_of_pressure(relative_position),
        area(area),
        chord(area / span),
        wingspan(span),
        normal(normal),
        aspect_ratio(physics::sq(span) / area),
        flap_ratio(flap_ratio)
    {
    }

    Wing(const glm::vec3& position, float span, float chord, const Airfoil* airfoil, const glm::vec3& normal,
        float flap_ratio = 0.25f)
        : airfoil(airfoil),
        center_of_pressure(position),
        area(span* chord),
        chord(chord),
        wingspan(span),
        normal(normal),
        aspect_ratio(physics::sq(span) / area),
        flap_ratio(flap_ratio)
    {
    }

    // controls how much the wing is deflected
    void set_control_input(float input) { control_input = glm::clamp(input, -1.0f, 1.0f); }

    // Compute and apply aerodynamic forces
    void apply_forces(RigidBody* rigid_body, double dt) {
        // Calculate the local velocity at the center of pressure. The center of pressure
        // is where aerodynamic forces are considered to act on the body.
        glm::vec3 local_velocity = rigid_body->get_point_velocity(center_of_pressure);
        float speed = glm::length(local_velocity);

        // If the speed is very low, aerodynamic forces can be negligible.
        if (speed <= 1.0f) return;

        // Drag acts in the opposite direction to the local airflow, i.e., the local velocity.
        // This is a fundamental principle of aerodynamics where drag opposes the direction of movement.
        glm::vec3 drag_direction = glm::normalize(-local_velocity);

        // Lift is perpendicular to the relative airflow direction (drag direction).
        // This is calculated by taking the cross product of the drag direction with the
        // normal (representing the airfoil's orientation), and then the cross product of
        // that result with the drag direction again. This ensures that lift is perpendicular
        // to the airflow direction.
        glm::vec3 lift_direction = glm::normalize(glm::cross(glm::cross(drag_direction, normal), drag_direction));

        // Calculate the angle of attack, which is the angle between the chord line of the
        // airfoil (represented by 'normal') and the airflow direction (represented by
        // 'drag_direction'). This angle is critical in determining the aerodynamic forces.
        float angle_of_attack = glm::degrees(std::asin(glm::dot(drag_direction, normal)));

        float lift_coeff, drag_coeff;
        // Sample aerodynamic coefficients from an airfoil model based on the angle of attack.
        // These coefficients are typically obtained from empirical data or theoretical models
        // and are essential for calculating lift and drag forces.
        std::tie(lift_coeff, drag_coeff) = airfoil->sample(angle_of_attack);

        // Adjust the lift coefficient based on the deflection of control surfaces (flaps).
        // The flap_ratio represents how much of the wing area is covered by flaps.
        // The deflection_ratio is the control input (how much the flaps are deflected).
        // The lift coefficient increases with flap deflection, enhancing the lifting capability.
        if (flap_ratio > 0.0f) {
            float deflection_ratio = control_input;
            float delta_lift_coeff = sqrt(flap_ratio) * airfoil->cl_max * deflection_ratio;
            lift_coeff += delta_lift_coeff;
        }

        // Compute induced drag, an additional drag that occurs due to lift generation.
        // It is significant at high angles of attack and high lift coefficients.
        // Induced drag is inversely proportional to the aspect ratio of the wing
        // and the efficiency factor, which accounts for the wing's shape and design.
        float induced_drag_coeff = physics::sq(lift_coeff) / (physics::PI * aspect_ratio * efficiency_factor);
        drag_coeff += induced_drag_coeff;

        // Air density varies with altitude. This factor is crucial for calculating
        // dynamic pressure and, consequently, the aerodynamic forces.
        float air_density = isa::get_air_density(rigid_body->position.y);

        // Dynamic pressure represents the kinetic energy per unit volume of the airflow.
        // It is a function of the air density and the square of the speed.
        float dynamic_pressure = 0.5f * physics::sq(speed) * air_density * area;

        // Compute the lift force as a product of the lift direction, lift coefficient,
        // and dynamic pressure. This force is perpendicular to the airflow direction.
        glm::vec3 lift = lift_direction * lift_coeff * dynamic_pressure;

        // Compute the drag force in a similar manner, but in the direction opposing the airflow.
        glm::vec3 drag = drag_direction * drag_coeff * dynamic_pressure;

        // Finally, apply the calculated aerodynamic forces (lift and drag) at the center of pressure.
        // This point is where the forces are considered to act on the rigid body.
        rigid_body->add_force_at_point(lift + drag, center_of_pressure);
    }
};

#endif
