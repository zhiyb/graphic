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
	display->vertices().push_back(Vector3D(0.7, -0.2, 0.f));
	display->vertices().push_back(Vector3D(0.f, 0.8, 0.f));
	display->vertices().push_back(Vector3D(-0.6, 0.3, 0.8));
	display->colours().push_back(Vector3D(1.f, 0.f, 0.f));
	display->colours().push_back(Vector3D(0.f, 1.f, 0.f));
	display->colours().push_back(Vector3D(0.f, 0.f, 1.f));

	display->clear();
	display->drawArray(Display::Triangles, 0, 3);
	display->update();

	delete display;
	return 0;
}
