ifeq ($(shell uname -m),armv6l)
  CC = arm-linux-gnueabihf-gcc
  CFLAGS = -g -gdwarf-2 -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
else
  ifeq ($(shell uname -m),armv7l)
    CC = gcc
    CFLAGS = -g -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -fPIC
  endif
endif

PREFIX   ?= /usr/local
CPPFLAGS = -Iinclude
LDFLAGS  = -L.
LDLIBS   = -lnrf24

NAME     = libnrf24
TESTNAME = test

all: lib examples

lib: src/gpio.o src/spi.o src/rf24.o
	$(CC) $(CPPFLAGS) -o $(NAME).so -shared -fPIC $(CFLAGS) src/gpio.o src/spi.o src/rf24.o

install: lib
	install -d $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include
	install -m 755 -o root -g root $(NAME).so $(DESTDIR)$(PREFIX)/lib/$(NAME).so
	install -m 644 -o root -g root include/* $(DESTDIR)$(PREFIX)/include

examples: pong_irq pong_curl

pong_irq: examples/pong_irq.o
	$(CC) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -o pong_irq $(CFLAGS) -lnrf24 examples/pong_irq.o

pong_curl: examples/pong_curl.o
	$(CC) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -o pong_curl $(CFLAGS) -lnrf24 -lcurl examples/pong_curl.o

clean:
	rm -f *.so examples/*.o src/*.o pong_irq pong_curl

.PHONY: clean
