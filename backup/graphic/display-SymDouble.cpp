#include <cmath>
#include <cstring>
#include <algorithm>
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

inline void Display::plot(const uint32_t x, const uint32_t y, const uint32_t reverse)
{
	if (reverse)
		*(buffer.data + x * width() + y) = buffer.colour;
	else
		*(buffer.data + y * width() + x) = buffer.colour;
}

void Display::drawRectangle(const Vector3D& p, const Vector3D& s)
{
	int x = p.x(), y = p.y(), w = s.x(), h = s.y();
	for (int dy = 0; dy < h; dy++)
		for (int dx = 0; dx < w; dx++)
			*(buffer.data + (y + dy) * width() + x + dx) = buffer.colour;
}

void Display::drawPoint(const Vector3D& p, const uint32_t size)
{
	drawRectangle(Vector3D(p.x() - size / 2, p.y() - size / 2, p.z()), Vector3D(size, size, 0));
}

void Display::drawLine(const Vector3D& p1, const Vector3D& p2)
{
	// Symmetric Double Step
	int a1 = p1.x(), b1 = p1.y(), a2 = p2.x(), b2 = p2.y();
	int dx, dy, incr1, incr2, d, x, y, xend, c, pixels_left;
	int x1, y1;
	int sign_x, sign_y, step, reverse, i;

	sign_x = (a2 - a1) < 0 ? -1 : 1;
	dx = (a2 - a1) * sign_x;
	sign_y = (b2 - b1) < 0 ? -1 : 1;
	dy = (b2 - b1) * sign_y;
	// Decide increment sign by the slope sign
	if (sign_x == sign_y)
		step = 1;
	else
		step = -1;

	// Choose axis of greatest movement (make * dx)
	if (dy > dx) {
		swap(a1, b1);
		swap(a2, b2);
		swap(dx, dy);
		reverse = 1;
	} else
		reverse = 0;
	// Note! Error check for dx == 0 should be included here
	// Start from the smaller coordinate
	if (a1 > a2) {
		x = a2;
		y = b2;
		x1 = a1;
		y1 = b1;
	} else {
		x = a1;
		y = b1;
		x1 = a2;
		y1 = b2;
	}

	// Note! dx = n implies 0 - n or (dx + 1) pixels to be set
	// Go round loop dx / 4 times then plot last 0, 1, 2 or 3 pixels
	// In fact (dx - 1) / 4 as 2 pixels are already plotted
	xend = (dx - 1) / 4;
	// Number of pixels left over at the end
	pixels_left = (dx - 1) % 4;
	plot(x, y, reverse);
	// Plot only one pixel for zero length vectors
	if (pixels_left < 0)
		return;

	// Plot first two pints
	plot(x1, y1, reverse);
	incr2 = 4 * dy - 2 * dx;
	// Slope less than 1/2
	if (incr2 < 0) {
		c = 2 * dy;
		incr1 = 2 * c;
		d = incr1 - dx;
		
		for (i = 0; i < xend; i++) {	// Plotting loop
			++x;
			--x1;
			if (d < 0) {
				// Pattern 1 forwards
				plot(x, y, reverse);
				plot(++x, y, reverse);
				// Pattern 1 backwards
				plot(x1, y1, reverse);
				plot(--x1, y1, reverse);
				d += incr1;
			} else {
				if (d < c) {
					// Pattern 2 forwards
					plot(x, y, reverse);
					plot(++x, y += step, reverse);
					// Pattern 2 backwards
					plot(x1, y1, reverse);
					plot(--x1, y1 -= step, reverse);
				} else {
					// Pattern 3 forwards
					plot(x, y += step, reverse);
					plot(++x, y, reverse);
					// Pattern 3 backwards
					plot(x1, y1 -= step, reverse);
					plot(--x1, y1, reverse);
				}
				d += incr2;
			}
		}

		// Plot last pattern
		if (pixels_left) {
			if (d < 0) {
				// Pattern 1
				plot(++x, y, reverse);
				if (pixels_left > 1)
					plot(++x, y, reverse);
				if (pixels_left > 2)
					plot(--x1, y1, reverse);
			} else {
				if (d < c) {
					// Pattern 2
					plot(++x, y, reverse);
					if (pixels_left > 1)
						plot(++x, y += step, reverse);
					if (pixels_left > 2)
						plot(--x1, y1, reverse);
				} else {
					// Pattern 3
					plot(++x, y += step, reverse);
					if (pixels_left > 1)
						plot(++x, y, reverse);
					if (pixels_left > 2)
						plot(--x1, y1 -= step, reverse);
				}
			}
		}
	} else {
		// Slope greater than 1/2
		c = 2 * (dy - dx);
		incr1 = 2 * c;
		d = incr1 + dx;
		
		for (i = 0; i < xend; i++) {	// Plotting loop
			++x;
			--x1;
			if (d > 0) {
				// Pattern 4 forwards
				plot(x, y += step, reverse);
				plot(++x, y += step, reverse);
				// Pattern 4 backwards
				plot(x1, y1 -= step, reverse);
				plot(--x1, y1 -= step, reverse);
				d += incr1;
			} else {
				if (d < c) {
					// Pattern 2 forwards
					plot(x, y, reverse);
					plot(++x, y += step, reverse);
					// Pattern 2 backwards
					plot(x1, y1, reverse);
					plot(--x1, y1 -= step, reverse);
				} else {
					// Pattern 3 forwards
					plot(x, y += step, reverse);
					plot(++x, y, reverse);
					// Pattern 3 backwards
					plot(x1, y1 -= step, reverse);
					plot(--x1, y1, reverse);
				}
				d += incr2;
			}
		}

		// Plot last pattern
		if (pixels_left) {
			if (d > 0) {
				// Pattern 4
				plot(++x, y += step, reverse);
				if (pixels_left > 1)
					plot(++x, y += step, reverse);
				if (pixels_left > 2)
					plot(--x1, y1 -= step, reverse);
			} else {
				if (d < c) {
					// Pattern 2
					plot(++x, y, reverse);
					if (pixels_left > 1)
						plot(++x, y += step, reverse);
					if (pixels_left > 2)
						plot(--x1, y1, reverse);
				} else {
					// Pattern 3
					plot(++x, y += step, reverse);
					if (pixels_left > 1)
						plot(++x, y, reverse);
					if (pixels_left > 2) {
						if (d > c)	// Step 3
							plot(--x1, y1 -= step, reverse);
						else		// Step 2
							plot(--x1, y1, reverse);
					}
				}
			}
		}
	}
}
