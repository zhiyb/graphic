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
	Vector3D c(0.f, 0.f, 0.f);
	for (double a = 0; a < 2.f * PI; a += PI * 2 / 120) {
		Vector3D p(r * cos(a), r * sin(a), 0);
		display->vertices().push_back(c + p / 3);
		display->vertices().push_back(c + p);
		display->colours().push_back(Vector3D());
		display->colours().push_back(Vector3D());
	}
	display->vertices().push_back(Vector3D());
	display->vertices().push_back(Vector3D(0.8, 0.8, 0.8));
	display->colours().push_back(Vector3D(1.f, 1.f, 1.f));
	display->colours().push_back(Vector3D(102.f / 255.f, 204.f / 255.f, 1.f));

start:
	for (int of = 0; of < 120; of++) {
		display->clear();
		Matrix4x4 m;
		display->setModelView(m);
		display->drawArray(Display::Lines, 240, 2);
		m.rotate(PI * 2.f * (float)of / 120.f, Vector3D(1, 1, 0));
		display->setModelView(m);
		for (int i = 0; i < 120; i++) {
			float o = (float)((i - of + 120) % 120) / 120.f;
			display->colour(i * 2) = Vector3D(o, 1.f - o, 0);
			display->colour(i * 2 + 1) = Vector3D(0, 0, o);
			display->drawArray(Display::Lines, i * 2, 2);
		}
		display->update();
		usleep(100000);
	}
	goto start;

	delete display;
	return 0;
}
