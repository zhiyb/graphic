#ifndef GRAPHIC_VECTOR4D
#define GRAPHIC_VECTOR4D

#include <inttypes.h>
#include "vector3d.h"

class Vector4D
{
public:
	Vector4D(void) {data.x = 0; data.y = 0; data.z = 0; data.w = 1;}
	Vector4D(float x, float y, float z, float w);
	Vector4D(const Vector3D& v, float w = 1.f);
	inline float x(void) const {return data.x;}
	inline float y(void) const {return data.y;}
	inline float z(void) const {return data.z;}
	inline float w(void) const {return data.w;}
	inline void setX(float x) {data.x = x;}
	inline void setY(float y) {data.y = y;}
	inline void setZ(float z) {data.z = z;}
	inline void setW(float w) {data.w = w;}
	inline uint32_t toColour(void) const;
	Vector4D& operator+=(const Vector4D& vector);
	Vector4D& operator-=(const Vector4D& vector);
	Vector4D& operator*=(const Vector4D& vector);
	Vector4D& operator/=(const Vector4D& vector);
	Vector4D& operator*=(float m);
	Vector4D& operator/=(float d);

private:
	struct {
		float x, y, z, w;
	} data;
};

const Vector4D operator+(const Vector4D& v1, const Vector4D& v2);
const Vector4D operator-(const Vector4D& v1, const Vector4D& v2);
const Vector4D operator*(const Vector4D& v1, const Vector4D& v2);
const Vector4D operator/(const Vector4D& v1, const Vector4D& v2);
const Vector4D operator-(const Vector4D& vector);
const Vector4D operator*(const Vector4D& v1, float m);
const Vector4D operator/(const Vector4D& v1, float d);

// inline functions

inline uint32_t Vector4D::toColour(void) const
{
	uint32_t r = 255 * x(), g = 255 * y(), b = 255 * z();
	return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

#endif
