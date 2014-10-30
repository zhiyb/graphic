#include <cmath>
#include <cstring>
#include <algorithm>
#include "display.h"

//#define ROUND(x)	(x >= 0 ? (long)(x + 0.5) : (long)(x - 0.5))

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
	resize(width, height);
}

void Display::resize(const int width, const int height)
{
	buffer.resize(width, height);
	data.mapping = Matrix4x4();
	data.mapping.scale(Vector3D(width / 2, -height / 2, 1.f));
	data.mapping.translate(Vector3D(width / 2, height / 2, 0.f));
	data.updateMVP();
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
		for (; count > 1; count -= 2) {
			drawLine(first, first + 1);
			first += 2;
		}
		break;
	case Triangles:
		for (; count > 1; count -= 3) {
			drawTriangle(first, first + 1, first + 2);
			first += 3;
		}
		break;
	}
}

const Vector3D Display::map(const Vector4D& v)
{
	return Vector3D(data.mvp * v);
}

void Display::drawLine(uint32_t index1, uint32_t index2)
{
	// Symmetric Double Step
	const Vector3D v1 = map(vertex(index1)), v2 = map(vertex(index2));
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
		clr = colour(index2);
		dclr = colour(index1) - clr;
	} else {
		x = a1;
		y = b1;
		x1 = a2;
		y1 = b2;
		z = v1.z();
		dz = v2.z() - z;
		clr = colour(index1);
		dclr = colour(index2) - clr;
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

void Display::drawTriangle(uint32_t index1, uint32_t index2, uint32_t index3)
{
#if 0
	drawLine(index1, index2);
	drawLine(index1, index3);
	drawLine(index2, index3);
#endif
	Vector3D v1 = map(vertex(index1)), v2 = map(vertex(index2)), v3 = map(vertex(index3));
	// swap to clockwise order, with index1 at the top
	if (v1.y() > v2.y()) {
		swap(v1, v2);
		//swap(index1, index2);
	}
	if (v1.y() > v3.y()) {
		swap(v1, v3);
		//swap(index1, index3);
	}
	// Note! y-axis is inverted
	// Check gradient for swap index2, index3
	if ((v2 - v1).normalized().x() > (v3 - v1).normalized().x()) {
//	if ((v2.y() - v1.y()) * (v1.x() - v3.x()) < (v3.y() - v1.y()) * (v1.x() - v2.x())) {
		swap(v2, v3);
		//swap(index2, index3);
	}
#if 0
	drawLine(index1, index2);
	drawLine(index1, index3);
	drawLine(index2, index3);
#endif
	//plot(v2.x(), v2.y(), false, 0, Vector3D(1.f, 1.f, 1.f));
	float dx12 = v2.x() - v1.x(), dx13 = v3.x() - v1.x(), dx23 = v3.x() - v2.x();
	float dy12 = v2.y() - v1.y(), dy13 = v3.y() - v1.y(), dy23 = v3.y() - v2.y();
	float dz12 = v2.z() - v1.z(), dz13 = v3.z() - v1.z(), dz23 = v3.z() - v2.z();
	Vector3D c1 = colour(index1), c2 = colour(index2), c3 = colour(index3);
	Vector3D dc12 = c2 - c1, dc13 = c3 - c1, dc23 = c3 - c2;
	Vector3D cL, cR;
	//int x1, y1, x2, y2;
	int y, yu, yd;
	float dy, dyL, dyR;
	float xL, xR;
	float zL, zR;
	float fL, fR;
	if (dy23 >= 0) {
		// v2 above v3
		yu = round(v2.y()) - 1;
		yd = round(v3.y()) - 1;
	} else {
		// v2 below v3
		yu = round(v3.y()) - 1;
		yd = round(v2.y()) - 1;
	}
#if 1
	// Upper part
	for (y = round(v1.y()); y <= yu; y++) {
		dy = (float)y + 0.5 - v1.y();
		fL = dy / dy12;
		fR = dy / dy13;
		xL = fL * dx12 + v1.x();
		xR = fR * dx13 + v1.x();
		if (round(xL) == round(xR))	// (round(xL) > round(xR) - 1)
			continue;
		zL = fL * dz12 + v1.z();
		zR = fR * dz13 + v1.z();
		cL = fL * dc12 + c1;
		cR = fR * dc13 + c1;
		drawHorizontalLine(y, xL, xR, zL, zR, cL, cR);
	}
#else
	y = yu + 1;
#endif
	// Lower part
	if (dy23 < 0) {
		return;
		// v2 above v3
		for (; y <= yd; y++) {
			dyL = (float)y + 0.5 - v2.y();
			dyR = (float)y + 0.5 - v1.y();
			fL = dyL / dy23;
			fR = dyR / dy13;
			xL = fL * dx23 + v2.x();
			xR = fR * dx13 + v1.x();
			if (round(xL) == round(xR))
				continue;
			zL = fL * dz23 + v2.z();
			zR = fR * dz13 + v1.z();
			cL = fL * dc23 + c2;
			cR = fR * dc13 + c1;
			drawHorizontalLine(y, xL, xR, zL, zR, cL, cR);
		}
	} else {
		return;
		// v3 above v2
		for (; y <= yd; y++) {
			dyL = (float)y + 0.5 - v1.y();
			dyR = (float)y + 0.5 - v3.y();
			fL = dyL / dy12;
			fR = dyR / -dy23;
			xL = fL * dx12 + v1.x();
			xR = fR * -dx23 + v3.x();
			if (round(xL) == round(xR))
				continue;
			zL = fL * dz12 + v1.z();
			zR = fR * -dz23 + v3.z();
			cL = fL * dc12 + c1;
			cR = fR * -dc23 + c3;
			drawHorizontalLine(y, xL, xR, zL, zR, cL, cR);
		}
	}
}

void Display::drawHorizontalLine(int y, float xL, float xR, float zL, float zR, const Vector3D& cL, const Vector3D& cR)
{
	int x1 = round(xL), x2 = round(xR) - 1;
	float dx = xR - xL, dz = zR - zL;
	Vector3D dc = cR - cL;
	float f;
	while (x1 <= x2) {
		f = ((float)x1 + 0.5 - xL) / dx;
		plot(x1++, y, false, f * dz + zL, f * dc + cL);
	}
}
