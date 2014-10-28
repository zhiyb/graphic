#ifndef GRAPHIC_MATRIX4X4
#define GRAPHIC_MATRIX4X4

#include <inttypes.h>
#include <cstring>

class Matrix4x4;

#include "vector3d.h"
#include "vector4d.h"

class Matrix4x4
{
public:
	Matrix4x4(void);
	Matrix4x4(const float* values) {memcpy(mat, values, 16 * sizeof(float));}
	float *data(void) {return mat;}
	const float *data(void) const {return mat;}
	const float *constData(void) const {return mat;}
	Vector4D map(const Vector4D& v) const;
	void rotate(float angle, const Vector3D& vector);
	void scale(const Vector3D& v);
	void translate(const Vector3D& v);

	const float& operator()(int row, int col) const {return *(mat + row + col * 4);}
	float& operator()(int row, int col) {return *(mat + row + col * 4);}
	Matrix4x4& operator*=(const Matrix4x4& m);

private:
	float mat[4 * 4];
};

const Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
const Vector4D operator*(const Matrix4x4& m, const Vector4D& v);

#endif
