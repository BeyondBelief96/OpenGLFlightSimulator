#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <numeric>
#include <type_traits>
#include <variant>
#include <vector>


//  International Standard Atmosphere (ISA)
namespace isa
{
	// get temperture in kelvin
	inline float get_air_temperature(float altitude)
	{
		/*assert(0.0f <= altitude && altitude <= 11000.0f);*/
		return 288.15f - 0.0065f * altitude;
	}

	// only accurate for altitudes < 11km
	float get_air_density(float altitude)
	{
		/*assert(0.0f <= altitude && altitude <= 11000.0f);*/
		float temperature = get_air_temperature(altitude);
		float pressure = 101325.0f * std::pow(1 - 0.0065f * (altitude / 288.15f), 5.25f);
		return 0.00348f * (pressure / temperature);
	}

	const float sea_level_air_density = get_air_density(0.0f);
};  // namespace isa

namespace physics
{
	// constants
	constexpr float EPSILON = 1e-8f;
	constexpr float EARTH_GRAVITY = 9.80665f;
	constexpr float PI = 3.141592653589793f;

	// directions in body space
	constexpr glm::vec3 X_AXIS = { 1.0f, 0.0f, 0.0f };
	constexpr glm::vec3 Y_AXIS = { 0.0f, 1.0f, 0.0f };
	constexpr glm::vec3 Z_AXIS = { 0.0f, 0.0f, 1.0f };

	constexpr glm::vec3 FORWARD = -Z_AXIS;
	constexpr glm::vec3 UP = +Y_AXIS;
	constexpr glm::vec3 RIGHT = +X_AXIS;
	constexpr glm::vec3 BACKWARD = +Z_AXIS;
	constexpr glm::vec3 DOWN = -Y_AXIS;
	constexpr glm::vec3 LEFT = -X_AXIS;

	// utility functions
	// x^2
	template <typename T>
	constexpr inline T sq(T x)
	{
		return x * x;
	}

	// x^3
	template <typename T>
	constexpr inline T cb(T x)
	{
		return x * x * x;
	}

	template <typename T>
	constexpr inline T scale(T input, T in_min, T in_max, T out_min, T out_max)
	{
		input = glm::clamp(input, in_min, in_max);
		return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	template <typename T>
	constexpr inline T lerp(T a, T b, float t)
	{
		t = glm::clamp(t, 0.0f, 1.0f);
		return a + t * (b - a);
	}

	template <typename T>
	constexpr inline float inverse_lerp(T a, T b, T v)
	{
		v = glm::clamp(v, a, b);
		return (v - a) / (b - a);
	}

	// inertia tensor calculations
	namespace inertia
	{
		// mass element used for inertia tensor calculation
		struct Element {
			glm::vec3 size;
			glm::vec3 position;  // position in design coordinates
			glm::vec3 inertia;   // moment of inertia
			glm::vec3 offset;    // offset from center of gravity
			float mass;
			float volume() const { return size.x * size.y * size.z; }
		};

		// cuboid moment of inertia
		inline glm::vec3 cuboid(float mass, const glm::vec3& size)
		{
			float x = size.x, y = size.y, z = size.z;
			return glm::vec3(sq(y) + sq(z), sq(x) + sq(z), sq(x) + sq(y)) * (1.0f / 12.0f) * mass;
		}

		// sphere moment of inertia
		inline glm::vec3 sphere(float mass, float radius) { return glm::vec3((2.0f / 5.0f) * mass * sq(radius)); }

		// inertia tensor from moment of inertia
		inline glm::mat3 tensor(const glm::vec3& moment_of_inertia) { return glm::diagonal3x3(moment_of_inertia); }

		// distribute mass among elements depending on element volume, to be called before passing elements to tensor()
		inline void set_uniform_density(std::vector<Element>& elements, float total_mass)
		{
			auto f = [](float s, auto& e) { return s + e.volume(); };
			float total_volume = std::accumulate(elements.begin(), elements.end(), 0.0f, f);

			for (auto& element : elements) {
				element.mass = (element.volume() / total_volume) * total_mass;
			}
		}

		// calculate inertia tensor for a collection of connected masses
		inline glm::mat3 tensor(std::vector<Element>& elements, bool precomputed_offset = false, glm::vec3* cg = nullptr)
		{
			float Ixx = 0, Iyy = 0, Izz = 0;
			float Ixy = 0, Ixz = 0, Iyz = 0;

			float mass = 0;
			glm::vec3 moment_of_inertia(0.0f);

			for (const auto& element : elements) {
				mass += element.mass;
				moment_of_inertia += element.mass * element.position;
			}

			const auto center_of_gravity = moment_of_inertia / mass;

			for (auto& element : elements) {
				if (!precomputed_offset) {
					element.offset = element.position - center_of_gravity;
				}
				else {
					element.offset = element.position;
				}

				const auto offset = element.offset;

				Ixx += element.inertia.x + element.mass * (sq(offset.y) + sq(offset.z));
				Iyy += element.inertia.y + element.mass * (sq(offset.z) + sq(offset.x));
				Izz += element.inertia.z + element.mass * (sq(offset.x) + sq(offset.y));
				Ixy += element.mass * (offset.x * offset.y);
				Ixz += element.mass * (offset.x * offset.z);
				Iyz += element.mass * (offset.y * offset.z);
			}

			if (cg != nullptr) {
				*cg = center_of_gravity;
			}

			// clang-format off
			return {
				Ixx, -Ixy, -Ixz,
				-Ixy, Iyy, -Iyz,
				-Ixz, -Iyz, Izz
			};
			// clang-format on
		}

		// helper function for the creation of a cuboid mass element
		inline Element cube(const glm::vec3& position, const glm::vec3& size, float mass = 0.0f)
		{
			glm::vec3 inertia = cuboid(mass, size);
			return { size, position, inertia, position, mass };
		}

	};  // namespace inertia

	// unit conversions
	namespace units
	{
		constexpr inline float knots(float meter_per_second) { return meter_per_second * 1.94384f; }

		constexpr inline float meter_per_second(float kilometer_per_hour) { return kilometer_per_hour / 3.6f; }

		constexpr inline float kilometer_per_hour(float meter_per_second) { return meter_per_second * 3.6f; }

		constexpr inline float kelvin(float celsius) { return celsius - 273.15f; }

		constexpr inline float watts(float horsepower) { return horsepower * 745.7f; }

		constexpr inline float mile_to_kilometre(float mile) { return mile * 1.609f; }

		constexpr inline float feet_to_meter(float feet) { return feet * 0.3048f; }

	};  // namespace units
}


#endif
