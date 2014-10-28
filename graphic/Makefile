SRC	= ascii.c framebuffer.cpp display.cpp vector3d.cpp
SUBDIRS	= 
TARGET	= graphic
OBJS	= $(subst .c,.o,$(subst .cpp,.o,$(SRC)))

PREFIX	:= #mipsel-linux-
CC	:= $(PREFIX)g++
AR	:= $(PREFIX)ar
CFLAGS	:= -g -I. -Wall -Wsign-compare -Werror -O3

ARFLAGS	= rcs

.PHONE: all
all: subdirs lib$(TARGET).a

# Subdirectories

.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

lib$(TARGET).a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONE: clean
clean:
	for d in $(SUBDIRS); do (cd $$d; $(MAKE) clean); done
	rm -f lib$(TARGET).a $(OBJS) $(OBJS:.o=.d) *.d.*

-include $(OBJS:.o=.d)

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
