#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "framebuffer.h"

using namespace std;

FrameBuffer::FrameBuffer(const char *path, const int width, const int height) : Display(width, height)
{
	data.fd = open(path, O_RDWR);
	if (data.fd == -1) {
		cerr << "Cannot open frame buffer " << path << endl;
		return;
	}
	data.map = (uint32_t *)mmap(0, buffer.bytes(), \
			PROT_READ | PROT_WRITE, MAP_SHARED, data.fd, 0);
	memset(data.map, 0, buffer.bytes());
}

FrameBuffer::~FrameBuffer(void)
{
	if (valid()) {
		munmap(data.map, buffer.bytes());
		close(data.fd);
	}
}

void FrameBuffer::update(void)
{
	memcpy(data.map, buffer.data, buffer.bytes());
}
