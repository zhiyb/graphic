#include <cmath>
#include <cstring>
#include <algorithm>
#include "display.h"

Display *display = 0;

using namespace std;

Display::Buffer::~Buffer(void)
{
	delete [] data;
	delete [] depth;
}

void Display::Buffer::resize(const int w, const int h)
{
	width = w;
	height = h;
	delete [] data;
	delete [] depth;
	data = new uint32_t[bytes() / sizeof(uint32_t)];
	depth = new float[bytes() / sizeof(uint32_t)];
}

Display::Display(const int width, const int height)
{
	buffer.resize(width, height);
}

void Display::clear(void)
{
	memset(buffer.data, 0, buffer.bytes());
}

inline void Display::plot(uint32_t x, uint32_t y, bool reverse, float depth, const Vector3D& colour)
{
	if (reverse) {
		*(buffer.data + x * width() + y) = colour.toColour();
		*(buffer.depth + x * width() + y) = depth;
	} else {
		*(buffer.data + y * width() + x) = colour.toColour();
		*(buffer.depth + y * width() + x) = depth;
	}
}

void Display::drawPoint(uint32_t index)
{
	const Vector3D v = map(vertex(index));
	int x = v.x(), y = v.y(), z = v.z();
	uint32_t c = colour(index).toColour();
	uint32_t s = pointSize();
	for (uint32_t dy = 0; dy < s; dy++)
		for (uint32_t dx = 0; dx < s; dx++)
			if (x >= 0 && x < width() && y >= 0 && y < height()) {
				*(buffer.depth + y * width() + x) = z;
				*(buffer.data + y++ * width() + x++) = c;
			}
}

void Display::drawArray(DrawModes mode, uint32_t first, uint32_t count)
{
	switch (mode) {
	case Points:
		while (count--)
			drawPoint(first++);
		break;
	case Lines:
		for (;count > 1; count -= 2) {
			drawLine(first);
			first += 2;
		}
		break;
	case Triangles:
		;
	}
}

#include <iostream>
const Vector3D Display::map(const Vector4D& v)
{
	Vector3D vec(data.mvp * v);
#if 0
	for (int i = 0; i < 4; i++)
		cout << data.mvp(i, 0) << ' ' << data.mvp(i, 1) << ' ' << data.mvp(i, 2) << ' ' << data.mvp(i, 3) << endl;
	cout << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3] << ' ' << endl;
	cout << vec[0] << ' ' << vec[1] << ' ' << vec[2] << ' ' << endl;
#endif
	return vec;
	//return Vector3D(data.mvp * v);
}

void Display::drawLine(uint32_t index)
{
	// Symmetric Double Step
	const Vector3D v1 = map(vertex(index)), v2 = map(vertex(index + 1));
	int a1 = v1.x(), b1 = v1.y(), a2 = v2.x(), b2 = v2.y();
	int dx, dy, incr1, incr2, d, x, y, xend, c, pixels_left;
	int x1, y1;
	int sign_x, sign_y, step, reverse, i;
	int df, xs;
	float z, dz;
	Vector3D clr, dclr;

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
		z = v2.z();
		dz = v1.z() - z;
		clr = colour(index + 1);
		dclr = colour(index) - clr;
	} else {
		x = a1;
		y = b1;
		x1 = a2;
		y1 = b2;
		z = v1.z();
		dz = v2.z() - z;
		clr = colour(index);
		dclr = colour(index + 1) - clr;
	}

	// Note! dx = n implies 0 - n or (dx + 1) pixels to be set
	// Go round loop dx / 4 times then plot last 0, 1, 2 or 3 pixels
	// In fact (dx - 1) / 4 as 2 pixels are already plotted
	xend = (dx - 1) / 4;
	xs = x;
	// Number of pixels left over at the end
	pixels_left = (dx - 1) % 4;
	plot(x, y, reverse, z, clr);
	// Plot only one pixel for zero length vectors
	if (pixels_left < 0)
		return;

	// Plot first two pints
	plot(x1, y1, reverse, z + dz, clr + dclr);
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
				df = x - xs;
				plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
				df = ++x - xs;
				plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
				// Pattern 1 backwards
				df = x1 - xs;
				plot(x1, y1, reverse, z + dz * df / dx, clr + dclr * df / dx);
				df = --x1 - xs;
				plot(x1, y1, reverse, z + dz * df / dx, clr + dclr * df / dx);
				d += incr1;
			} else {
				if (d < c) {
					// Pattern 2 forwards
					df = x - xs;
					plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
					df = ++x - xs;
					plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
					// Pattern 2 backwards
					df = x1 - xs;
					plot(x1, y1, reverse, z + dz * df / dx, clr + dclr * df / dx);
					df = --x1 - xs;
					plot(x1, y1 -= step, reverse, z + dz * df / dx, clr + dclr * df / dx);
				} else {
					// Pattern 3 forwards
					df = x - xs;
					plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
					df = ++x - xs;
					plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
					// Pattern 3 backwards
					df = x1 - xs;
					plot(x1, y1 -= step, reverse, z + dz * df / dx, clr + dclr * df / dx);
					df = --x1 - xs;
					plot(x1, y1, reverse, z + dz * df / dx, clr + dclr * df / dx);
				}
				d += incr2;
			}
		}

		// Plot last pattern
		if (pixels_left) {
			if (d < 0) {
				// Pattern 1
				df = ++x - xs;
				plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
				if (pixels_left > 1) {
					df = ++x - xs;
					plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
				}
				if (pixels_left > 2) {
					df = --x1 - xs;
					plot(x1, y1, reverse, z + dz * df / dx, clr + dclr * df / dx);
				}
			} else {
				if (d < c) {
					// Pattern 2
					df = ++x - xs;
					plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
					if (pixels_left > 1) {
						df = ++x - xs;
						plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
					}
					if (pixels_left > 2) {
						df = --x1 - xs;
						plot(x1, y1, reverse, z + dz * df / dx, clr + dclr * df / dx);
					}
				} else {
					// Pattern 3
					df = ++x - xs;
					plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
					if (pixels_left > 1) {
						df = ++x - xs;
						plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
					}
					if (pixels_left > 2) {
						df = --x1 - xs;
						plot(x1, y1 -= step, reverse, z + dz * df / dx, clr + dclr * df / dx);
					}
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
				df = x - xs;
				plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
				df = ++x - xs;
				plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
				// Pattern 4 backwards
				df = x1 - xs;
				plot(x1, y1 -= step, reverse, z + dz * df / dx, clr + dclr * df / dx);
				df = --x1 - xs;
				plot(x1, y1 -= step, reverse, z + dz * df / dx, clr + dclr * df / dx);
				d += incr1;
			} else {
				if (d < c) {
					// Pattern 2 forwards
					df = x - xs;
					plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
					df = ++x - xs;
					plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
					// Pattern 2 backwards
					df = x1 - xs;
					plot(x1, y1, reverse, z + dz * df / dx, clr + dclr * df / dx);
					df = --x1 - xs;
					plot(x1, y1 -= step, reverse, z + dz * df / dx, clr + dclr * df / dx);
				} else {
					// Pattern 3 forwards
					df = x - xs;
					plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
					df = ++x - xs;
					plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
					// Pattern 3 backwards
					df = x1 - xs;
					plot(x1, y1 -= step, reverse, z + dz * df / dx, clr + dclr * df / dx);
					df = --x1 - xs;
					plot(x1, y1, reverse, z + dz * df / dx, clr + dclr * df / dx);
				}
				d += incr2;
			}
		}

		// Plot last pattern
		if (pixels_left) {
			if (d > 0) {
				// Pattern 4
				df = ++x - xs;
				plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
				if (pixels_left > 1) {
					df = ++x - xs;
					plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
				}
				if (pixels_left > 2) {
					df = --x1 - xs;
					plot(x1, y1 -= step, reverse, z + dz * df / dx, clr + dclr * df / dx);
				}
			} else {
				if (d < c) {
					// Pattern 2
					df = ++x - xs;
					plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
					if (pixels_left > 1) {
						df = ++x - xs;
						plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
					}
					if (pixels_left > 2) {
						df = --x1 - xs;
						plot(x1, y1, reverse, z + dz * df / dx, clr + dclr * df / dx);
					}
				} else {
					// Pattern 3
					df = ++x - xs;
					plot(x, y += step, reverse, z + dz * df / dx, clr + dclr * df / dx);
					if (pixels_left > 1) {
						df = ++x - xs;
						plot(x, y, reverse, z + dz * df / dx, clr + dclr * df / dx);
					}
					if (pixels_left > 2) {
						df = --x1 - xs;
						if (d > c)	// Step 3
							plot(x1, y1 -= step, reverse, z + dz * df / dx, clr + dclr * df / dx);
						else		// Step 2
							plot(x1, y1, reverse, z + dz * df / dx, clr + dclr * df / dx);
					}
				}
			}
		}
	}
}
