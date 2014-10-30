#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cmath>
#include <graphic.h>

int fd;
uint32_t *scrMap;

void init(void)
{
	display = new FrameBuffer("/dev/fb0", 800, 600);
}

int main(void)
{
	using namespace std;

	init();

	// Generate arrays
	const float coords[6][4][3] = {
		{{+0.5, +0.5, +0.5}, {+0.5, -0.5, +0.5}, {+0.5, -0.5, -0.5}, {+0.5, +0.5, -0.5}},
		{{-0.5, +0.5, -0.5}, {-0.5, +0.5, +0.5}, {+0.5, +0.5, +0.5}, {+0.5, +0.5, -0.5}},
		{{-0.5, +0.5, +0.5}, {-0.5, -0.5, +0.5}, {+0.5, -0.5, +0.5}, {+0.5, +0.5, +0.5}},
		{{-0.5, +0.5, -0.5}, {-0.5, -0.5, -0.5}, {-0.5, -0.5, +0.5}, {-0.5, +0.5, +0.5}},
		{{-0.5, -0.5, +0.5}, {-0.5, -0.5, -0.5}, {+0.5, -0.5, -0.5}, {+0.5, -0.5, +0.5}},
		{{+0.5, +0.5, -0.5}, {+0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5}, {-0.5, +0.5, -0.5}},
	};
	const float colours[6][3] = {
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1},
		{0, 1, 1},
		{1, 0, 1},
		{1, 1, 0},
	};
	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 4; j++) {
			display->vertices().push_back(Vector3D(coords[i][j][0], coords[i][j][1], coords[i][j][2]));
			display->colours().push_back(Vector3D(colours[i][0], colours[i][1], colours[i][2]));
		}

start:
	int ro = 360;
	for (int of = 0; of < ro; of++) {
		display->clear();
		Matrix4x4 m;
		m.rotate(PI * 2.f * (float)of / (float)ro, Vector3D(1, 1, 0));
		display->setModelView(m);
		for (int i = 0; i < 6; i++)
			display->drawArray(Display::TriangleFan, i * 4, 4);
		display->update();
		usleep(20000);
	}
	goto start;

	delete display;
	return 0;
}
