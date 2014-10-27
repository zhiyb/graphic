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

	display->clear();
	display->drawPoint(Vector3D(display->width() / 2, display->height() / 2, 0), 4, 0x66CCFF);
	display->update();

	delete display;
	return 0;
}
