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

start:
	for (double of = 0; of < 2 * PI; of += PI * 2 / 120) {
	display->clear();
	//double of = 1.2;
	int r = 250;
	Vector3D c(display->width() / 2, display->height() / 2, 0);
	for (double a = of; a < of + 2.f * PI; a += PI * 2 / 120) {
		uint32_t o = 0xFF * (a - of) / 2 / PI;
		display->setColour(o | (o << 8) | (o << 16));
		Vector3D p(c.x() + r * cos(a), c.y() + r * sin(a), 0);
		display->drawLine(c, p);
	}
	display->update();
	usleep(1000);
	}
	goto start;

	delete display;
	return 0;
}
