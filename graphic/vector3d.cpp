#include "vector3d.h"

Vector3D::Vector3D(float x, float y, float z)
{
	vec[0] = x;
	vec[1] = y;
	vec[2] = z;
}

Vector3D::Vector3D(const Vector4D& v)
{
	if (v.w() == 1) {
		vec[0] = v.x();
		vec[1] = v.y();
		vec[2] = v.z();
	} else {
		vec[0] = v.x() / v.w();
		vec[1] = v.y() / v.w();
		vec[2] = v.z() / v.w();
	}
}

Vector3D& Vector3D::operator+=(const Vector3D& vector)
{
	setX(x() + vector.x());
	setY(y() + vector.y());
	setZ(z() + vector.z());
	return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& vector)
{
	setX(x() - vector.x());
	setY(y() - vector.y());
	setZ(z() - vector.z());
	return *this;
}

Vector3D& Vector3D::operator*=(const Vector3D& vector)
{
	setX(x() * vector.x());
	setY(y() * vector.y());
	setZ(z() * vector.z());
	return *this;
}

Vector3D& Vector3D::operator/=(const Vector3D& vector)
{
	setX(x() / vector.x());
	setY(y() / vector.y());
	setZ(z() / vector.z());
	return *this;
}

Vector3D& Vector3D::operator*=(float m)
{
	setX(x() * m);
	setY(y() * m);
	setZ(z() * m);
	return *this;
}

Vector3D& Vector3D::operator/=(float d)
{
	setX(x() / d);
	setY(y() / d);
	setZ(z() / d);
	return *this;
}

const Vector3D operator+(const Vector3D& v1, const Vector3D& v2)
{
	Vector3D v(v1);
	v += v2;
	return v;
}

const Vector3D operator-(const Vector3D& v1, const Vector3D& v2)
{
	Vector3D v(v1);
	v -= v2;
	return v;
}

const Vector3D operator*(const Vector3D& v1, const Vector3D& v2)
{
	Vector3D v(v1);
	v *= v2;
	return v;
}

const Vector3D operator/(const Vector3D& v1, const Vector3D& v2)
{
	Vector3D v(v1);
	v /= v2;
	return v;
}

const Vector3D operator-(const Vector3D& vector)
{
	Vector3D v;
	v -= vector;
	return v;
}

const Vector3D operator*(const Vector3D& v1, float m)
{
	Vector3D v(v1);
	v *= m;
	return v;
}

const Vector3D operator/(const Vector3D& v1, float d)
{
	Vector3D v(v1);
	v /= d;
	return v;
}
