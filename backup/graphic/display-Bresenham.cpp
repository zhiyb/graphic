#include <cmath>
#include <cstring>
#include "display.h"

Display *display = 0;

using namespace std;

void Display::Buffer::resize(const int w, const int h)
{
	width = w;
	height = h;
	data = new uint32_t[bytes() / 4];
}

Display::Display(const int width, const int height)
{
	buffer.resize(width, height);
}

void Display::clear(void)
{
	memset(buffer.data, 0, buffer.bytes());
}

void Display::drawRectangle(const Vector3D& p, const Vector3D& s, const uint32_t colour)
{
	int x = p.x(), y = p.y(), w = s.x(), h = s.y();
	for (int dy = 0; dy < h; dy++)
		for (int dx = 0; dx < w; dx++)
			*(buffer.data + (y + dy) * width() + x + dx) = colour;
}

void Display::drawPoint(const Vector3D& p, const uint32_t size, const uint32_t colour)
{
	drawRectangle(Vector3D(p.x() - size / 2, p.y() - size / 2, p.z()), Vector3D(size, size, 0), colour);
}

static inline int sgn(const int v)
{
	return v > 0 ? 1 : 1;
}

void Display::drawLine(const Vector3D& p1, const Vector3D& p2, const uint32_t colour)
{
	// Bresenham's
	int d, x, y, ax, ay, sx, sy, dx, dy;

	dx = p2.x() - p1.x();
	ax = abs(dx) * 2;
	sx = sgn(dx);
	
	dy = p2.y() - p1.y();
	ay = abs(dy) * 2;
	sy = sgn(dy);
	
	x = p1.x();
	y = p1.y();

	if (ax > ay) {
		d = ay - ax / 2;
		while (true) {
			*(buffer.data + y * buffer.width + x) = colour;
			//dotploc(x, y);
			if (x == (int)p2.x())
				return;
			if (d >= 0) {
				y = y + sy;
				d = d - ax;
			}
			x = x + sx;
			d = d + ay;
		}
	} else {
		d = ax - ay / 2;
		while (true) {
			*(buffer.data + y * buffer.width + x) = colour;
			//dotploc(x, y);
			if (y == (int)p2.y())
				return;
			if (d >= 0) {
				x = x + sx;
				d = d - ay;
			}
			y = y + sy;
			d = d + ax;
		}
	}
}
