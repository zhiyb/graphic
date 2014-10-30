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
	float r = 0.8;
	int cLines = 12;
	Vector3D center(0.f, 0.f, 0.2);
	for (double a = 0; a < 2.f * PI; a += PI * 2.f / (float)cLines) {
		Vector3D p(r * cos(a), r * sin(a), 0);
		display->vertices().push_back(center + p / 3);
		display->vertices().push_back(center + p);
		display->colours().push_back(Vector3D());
		display->colours().push_back(Vector3D());
	}
	//float r = 0.8;
	int c = 64;
	for (int i = 0; i < c;) {
		Vector3D pL(r * cos((float)i * 2.f * PI / (float)c), r * sin((float)i * 2.f * PI / (float)c), 0);
		i++;
		Vector3D pR(r * cos((float)i * 2.f * PI / (float)c), r * sin((float)i * 2.f * PI / (float)c), 0);
		display->vertices().push_back(Vector3D());
		display->vertices().push_back(Vector3D(pL));
		display->vertices().push_back(Vector3D(pR));
		display->colours().push_back(Vector3D(1.f, 0.f, 0.f));
		if (i % 2) {
			display->colours().push_back(Vector3D(0.f, 1.f, 0.f));
			display->colours().push_back(Vector3D(0.f, 0.f, 1.f));
		} else {
			display->colours().push_back(Vector3D(0.f, 0.f, 1.f));
			display->colours().push_back(Vector3D(0.f, 1.f, 0.f));
		}
	}

start:
	int ro = 360;
	for (int of = 0; of < ro; of++) {
		display->clear();
		Matrix4x4 m;
		m.rotate(PI * 2.f * (float)of / (float)ro, Vector3D(1, 1, 0));
		display->setModelView(m);
		for (int i = 0; i < cLines; i++) {
			float o = (float)((i + of) % cLines) / (float)cLines;
			display->colour(i * 2) = Vector3D(o, 1.f - o, 0);
			display->colour(i * 2 + 1) = Vector3D(0, 0, o);
		}
		display->drawArray(Display::TriangleStrip, 0, cLines * 2);
		for (int i = 0; i < c; i++)
			display->drawArray(Display::Triangles, cLines * 2 + i * 3, 3);
		display->update();
		usleep(20000);
	}
	goto start;

	delete display;
	return 0;
}
