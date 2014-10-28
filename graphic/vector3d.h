#ifndef GRAPHIC_VECTOR3D
#define GRAPHIC_VECTOR3D

#include <inttypes.h>

class Vector3D;

#include "vector4d.h"

class Vector3D
{
public:
	Vector3D(void) {data.x = 0; data.y = 0; data.z = 0;}
	Vector3D(float x, float y, float z);
	Vector3D(const Vector4D& v);
	inline float x(void) const {return data.x;}
	inline float y(void) const {return data.y;}
	inline float z(void) const {return data.z;}
	inline void setX(float x) {data.x = x;}
	inline void setY(float y) {data.y = y;}
	inline void setZ(float z) {data.z = z;}
	inline uint32_t toColour(void) const;

	Vector3D& operator+=(const Vector3D& vector);
	Vector3D& operator-=(const Vector3D& vector);
	Vector3D& operator*=(const Vector3D& vector);
	Vector3D& operator/=(const Vector3D& vector);
	Vector3D& operator*=(float m);
	Vector3D& operator/=(float d);

private:
	struct {
		float x, y, z;
	} data;
};

const Vector3D operator+(const Vector3D& v1, const Vector3D& v2);
const Vector3D operator-(const Vector3D& v1, const Vector3D& v2);
const Vector3D operator*(const Vector3D& v1, const Vector3D& v2);
const Vector3D operator/(const Vector3D& v1, const Vector3D& v2);
const Vector3D operator-(const Vector3D& vector);
const Vector3D operator*(const Vector3D& v1, float m);
const Vector3D operator/(const Vector3D& v1, float d);

// inline functions

inline uint32_t Vector3D::toColour(void) const
{
	uint32_t r = 255 * x(), g = 255 * y(), b = 255 * z();
	return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

#endif
