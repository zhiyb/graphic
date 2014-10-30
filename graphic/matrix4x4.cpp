#include <cmath>
#include "matrix4x4.h"

using namespace std;

Matrix4x4::Matrix4x4(void)
{
	float values[16] = {
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f,
	};
	memcpy(mat, values, 16 * sizeof(float));
}

Vector4D Matrix4x4::map(const Vector4D& v) const
{
	Vector4D vec(0, 0, 0, 0);
	for (int r = 0; r < 4; r++)
		for (int i = 0; i < 4; i++)
			vec[r] += (*this)(r, i) * v[i];
	return vec;
}

void Matrix4x4::rotate(float angle, const Vector3D& vector)
{
	Vector3D v = vector.normalized();
	float c = cos(angle), s = sin(angle);
	float x = v.x(), y = v.y(), z = v.z();
	float xs = x * s, ys = y * s, zs = z * s;
	float xy1c = x * y * (1.f - c);
	float xz1c = x * z * (1.f - c);
	float yz1c = y * z * (1.f - c);
	float m[16] = {
		c + x * x * (1.f - c), xy1c + zs, xz1c - ys, 0.f,
		xy1c - zs, c + y * y * (1.f - c), yz1c + xs, 0.f,
		xz1c + ys, yz1c - xs, c + z * z * (1.f - c), 0.f,
		0.f, 0.f, 0.f, 1.f,
	};
	(*this) *= Matrix4x4(m);
}

void Matrix4x4::scale(const Vector3D& v)
{
	float m[16] = {
		v.x(), 0.f, 0.f, 0.f,
		0.f, v.y(), 0.f, 0.f,
		0.f, 0.f, v.z(), 0.f,
		0.f, 0.f, 0.f, 1.f,
	};
	(*this) *= Matrix4x4(m);
}

void Matrix4x4::translate(const Vector3D& v)
{
	float m[16] = {
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		v.x(), v.y(), v.z(), 1.f,
	};
	(*this) *= Matrix4x4(m);
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& m)
{
	float v[16];
	for (int r = 0; r < 4; r++)
		for (int c = 0; c < 4; c++) {
			*(v + r + 4 * c) = 0.f;
			for (int i = 0; i < 4; i++)
				*(v + r + 4 * c) += m(r, i) * (*this)(i, c);
		}
	memcpy(mat, v, 16 * sizeof(float));
	return *this;
}

const Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2)
{
	Matrix4x4 m(m2);
	m *= m1;
	return m;
}

const Vector4D operator*(const Matrix4x4& m, const Vector4D& v)
{
	return m.map(v);
}
