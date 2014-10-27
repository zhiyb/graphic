#ifndef GRAPHIC_FRAMEBUFFER_H
#define GRAPHIC_FRAMEBUFFER_H

#include <inttypes.h>
#include "display.h"

class FrameBuffer : public Display
{
public:
	FrameBuffer(const char *path, const int width, const int height);
	~FrameBuffer(void);
	void update(void);
	bool valid(void) {return data.fd != -1;}

private:
	struct {
		int fd;
		uint32_t *map;
	} data;
};

#endif
