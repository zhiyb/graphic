#ifndef GRAPHIC_DISPLAY_H
#define GRAPHIC_DISPLAY_H

#include <inttypes.h>
#include "vector.h"

class Display
{
public:
	Display(const int width, const int height);
	virtual ~Display(void) {}
	virtual void update(void) = 0;
	virtual bool valid(void) = 0;
	inline int width(void) const {return buffer.width;}
	inline int height(void) const {return buffer.height;}
	inline uint32_t colour(void) const {return buffer.colour;}
	inline void setColour(const uint32_t colour) {buffer.colour = colour;}
	void drawLine(const Vector3D& p1, const Vector3D& p2);
	void drawPoint(const Vector3D& p, const uint32_t size);
	void clear(void);

protected:
	struct Buffer {
		Buffer(void) : data(0) {}
		~Buffer(void) {delete [] data;}
		void resize(const int w, const int h);
		uint32_t bytes(void) {return width * height * 4;}
		uint32_t width, height;
		uint32_t *data;
		uint32_t colour;
	} buffer;

private:
	void drawRectangle(const Vector3D& p, const Vector3D& s);
	inline void plot(const uint32_t x, const uint32_t y, const uint32_t reverse);
};

extern Display *display;

#endif
