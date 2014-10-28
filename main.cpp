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
	int r = 250;
	Vector3D c(display->width() / 2, display->height() / 2, 0);
	for (double a = 0; a < 2.f * PI; a += PI * 2 / 120) {
		Vector3D p(r * cos(a), r * sin(a), 0);
		display->vertices().push_back(c + p / 3);
		display->vertices().push_back(c + p);
		display->colours().push_back(Vector3D());
		display->colours().push_back(Vector3D());
	}

start:
	for (int of = 0; of < 120; of++) {
		display->clear();
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
