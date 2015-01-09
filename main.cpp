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
	float len = 2.f;
	display->vertices().push_back(Vector3D());
	display->vertices().push_back(Vector3D(len, 0.f, 0.f));
	display->colours().push_back(Vector3D(1.f, 0.f, 0.f));
	display->colours().push_back(Vector3D(1.f, 0.f, 0.f));
	display->vertices().push_back(Vector3D());
	display->vertices().push_back(Vector3D(0.f, len, 0.f));
	display->colours().push_back(Vector3D(0.f, 1.f, 0.f));
	display->colours().push_back(Vector3D(0.f, 1.f, 0.f));
	display->vertices().push_back(Vector3D());
	display->vertices().push_back(Vector3D(0.f, 0.f, len));
	display->colours().push_back(Vector3D(0.f, 0.f, 1.f));
	display->colours().push_back(Vector3D(0.f, 0.f, 1.f));

	// Projection
	Matrix4x4 mp;
	mp.perspective(45.f, 800.f / 600.f, 0.1, 1000.0);
	mp.lookAt(Vector3D(0, 0, 3), Vector3D(0, 0, 0), Vector3D(0, 1, 0));
	//mp.rotate(PI * 36.f / 180.f, Vector3D(1, 0, 0));
	Vector3D vec(0.5, -0.599661, 6.37471);
	cout << vec.x() << ", " << vec.y() << ", " << vec.z() << endl;
	vec = mp * vec;
	cout << vec.x() << ", " << vec.y() << ", " << vec.z() << endl;
	//return 0;

start:
	int ro = 360;
	for (int of = 0; of < ro; of += 1) {
		display->clear();
		// Projection rotation
		Matrix4x4 p;
		p.rotate(PI * 2.0f * (float)of / (float)ro, Vector3D(1, 0, 0));
		display->setProjection(mp * p);
		// Model-View
		Matrix4x4 m;
		display->setModelView(m);
		// Axis
		display->drawArray(Display::Lines, 4 * 6, 6);
#if 1
		// Draw cube
		for (int i = 0; i < 6; i++)
			display->drawArray(Display::TriangleFan, i * 4, 4);
		// Another cube
		m.scale(Vector3D(-1.f / 3.f, -1.2, -1.2));
		m.translate(Vector3D(1.5f / 6.f, 0, 0));
		display->setModelView(m);
		for (int i = 0; i < 6; i++)
			display->drawArray(Display::TriangleFan, i * 4, 4);
		// Third cube
		m.translate(Vector3D(-3.f / 6.f, 0, 0));
		display->setModelView(m);
		for (int i = 0; i < 6; i++)
			display->drawArray(Display::TriangleFan, i * 4, 4);
#endif
		// Far away cube
		m = Matrix4x4();
		//m.scale(Vector3D(0.5, 0.5, 0.5));
		m.translate(Vector3D(0.f, 0.f, 6.f));
		display->setModelView(m);
		for (int i = 0; i < 6; i++)
			display->drawArray(Display::TriangleFan, i * 4, 4);
		// Redraw
		display->update();
		usleep(20000);
	}
	goto start;
		goto finish;

finish:
	delete display;
	return 0;
}
