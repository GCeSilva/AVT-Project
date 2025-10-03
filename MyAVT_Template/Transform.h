#pragma once
#include <array>

#define PI 3.14159265358979323846f

using vec3 = std::array<float, 3>;

struct Transform {
	vec3* translation;
	vec3* scale;
	// deprecated
	vec3* rotation;

	Transform operator+(const Transform& other) const {
		return Transform{
			new vec3{
				(translation ? (*translation)[0] : 0) + (other.translation ? (*other.translation)[0] : 0),
				(translation ? (*translation)[1] : 0) + (other.translation ? (*other.translation)[1] : 0),
				(translation ? (*translation)[2] : 0) + (other.translation ? (*other.translation)[2] : 0)
			},
			(!scale && !other.scale) ? nullptr : new vec3{
				(scale ? (*scale)[0] : 1) + (other.scale ? (*other.scale)[0] : 0),
				(scale ? (*scale)[1] : 1) + (other.scale ? (*other.scale)[1] : 0),
				(scale ? (*scale)[2] : 1) + (other.scale ? (*other.scale)[2] : 0),
			},
			(!rotation && !other.rotation) ? nullptr : new vec3{
				(rotation ? (*rotation)[0] : 0) + (other.rotation ? (*other.rotation)[0] * (PI / 180.0f) : 0),
				(rotation ? (*rotation)[1] : 0) + (other.rotation ? (*other.rotation)[1] * (PI / 180.0f) : 0),
				(rotation ? (*rotation)[2] : 0) + (other.rotation ? (*other.rotation)[2] * (PI / 180.0f) : 0),
			}
		};
	}
	Transform operator+=(const Transform& other) {
		*this = *this + other;
		return *this;
	}
};
