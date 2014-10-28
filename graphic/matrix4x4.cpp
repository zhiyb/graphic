#include "matrix4x4.h"

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
