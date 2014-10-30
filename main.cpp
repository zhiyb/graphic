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
	for (int i = 0; i < 8;) {
		Vector3D pL(r * cos((float)i * 2.f * PI / 8.f), r * sin((float)i * 2.f * PI / 8.f), 0);
		i++;
		Vector3D pR(r * cos((float)i * 2.f * PI / 8.f), r * sin((float)i * 2.f * PI / 8.f), 0);
		display->vertices().push_back(Vector3D());
		display->vertices().push_back(Vector3D(pL));
		display->vertices().push_back(Vector3D(pR));
		display->colours().push_back(Vector3D(1.f, 0.f, 0.f));
		display->colours().push_back(Vector3D(0.f, 1.f, 0.f));
		display->colours().push_back(Vector3D(0.f, 0.f, 1.f));
	}

	display->clear();
	for (int i = 0; i < 8; i++)
	//int i = 1;
		display->drawArray(Display::Triangles, i * 3, 3);
	display->update();

	delete display;
	return 0;
}
