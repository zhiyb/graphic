SRC	= main.cpp
SUBDIRS	= graphic
TARGET	= HelloWorld
STRIP	= false
OBJS	= $(subst .c,.o,$(subst .cpp,.o,$(SRC)))

PREFIX	= #mipsel-linux-
CC	:= $(PREFIX)g++
STRIP	+= && $(PREFIX)strip
CFLAGS	:= -g -I. -I./graphic -Wall -Wsign-compare -O3
LDFLAGS	:= -g -L./graphic -lgraphic -lpthread

.PHONE: all
all: $(TARGET)

# Subdirectories

.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

$(TARGET): subdirs $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
	$(STRIP) $@ || true

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONE: clean
clean:
	for d in $(SUBDIRS); do (cd $$d; $(MAKE) clean); done
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d) *.d.*

.PHONE: run
run: all
	./$(TARGET)

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
