#ifndef GRAPHIC_DISPLAY_H
#define GRAPHIC_DISPLAY_H

#include <inttypes.h>
#include <vector>
#include "vector3d.h"
#include "vector4d.h"
#include "matrix4x4.h"

class Display
{
public:
	enum DrawModes {Points, Lines, Triangles};
	Display(const int width, const int height);
	virtual ~Display(void) {}
	virtual void update(void) = 0;
	virtual bool valid(void) = 0;
	inline int width(void) const {return buffer.width;}
	inline int height(void) const {return buffer.height;}
	void drawArray(DrawModes mode, uint32_t first, uint32_t count);
	void clear(void);

	inline uint32_t pointSize(void) const {return data.pointSize;}
	inline void setPointSize(const uint32_t size) {data.pointSize = size;}
	inline Vector3D& vertex(const uint32_t index) {return vertices()[index];}
	inline Vector3D& colour(const uint32_t index) {return colours()[index];}
	inline std::vector<Vector3D>& vertices(void) {return data.vertices;}
	inline std::vector<Vector3D>& colours(void) {return data.colours;}
	const Matrix4x4& modelView(void) const {return data.modelView;}
	const Matrix4x4& projection(void) const {return data.projection;}
	void setModelView(const Matrix4x4& m) {data.modelView = m; data.updateMVP();}
	void setProjection(const Matrix4x4& m) {data.projection = m; data.updateMVP();}

protected:
	struct Buffer {
		Buffer(void) : data(0), depth(0) {}
		~Buffer(void);
		void resize(const int w, const int h);
		uint32_t bytes(void) {return width * height * sizeof(uint32_t);}
		uint32_t width, height;
		uint32_t *data;
		float *depth;
	} buffer;

private:
	struct Data {
		void updateMVP(void) {mvp = projection * modelView;}
		std::vector<Vector3D> vertices, colours;
		Matrix4x4 mvp, modelView, projection;
		uint32_t pointSize;
	} data;

	void drawLine(uint32_t index);
	void drawPoint(uint32_t index);
	inline void plot(uint32_t x, uint32_t y, bool reverse, float depth, const Vector3D& colour);
	const Vector3D map(const Vector4D& v);
};

extern Display *display;

#endif
