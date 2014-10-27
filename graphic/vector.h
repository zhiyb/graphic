#ifndef GRAPHIC_VECTOR
#define GRAPHIC_VECTOR

class Vector3D
{
public:
	Vector3D(void) {}
	Vector3D(float x, float y, float z);
	float x(void) const {return data.x;}
	float y(void) const {return data.y;}
	float z(void) const {return data.z;}
	void setX(float x) {data.x = x;}
	void setY(float y) {data.y = y;}
	void setZ(float z) {data.z = z;}

private:
	struct {
		float x, y, z;
	} data;
};

#endif
