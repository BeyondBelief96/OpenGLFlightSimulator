#ifndef MATH_H
#define MATH_H

#include <glm/glm.hpp>

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

	constexpr glm::vec3 FORWARD = +X_AXIS;
	constexpr glm::vec3 UP = +Y_AXIS;
	constexpr glm::vec3 RIGHT = +Z_AXIS;
	constexpr glm::vec3 BACKWARD = -X_AXIS;
	constexpr glm::vec3 DOWN = -Y_AXIS;
	constexpr glm::vec3 LEFT = -Z_AXIS;

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

}


#endif
