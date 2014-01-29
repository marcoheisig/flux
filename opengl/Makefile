UNAME_S := $(shell uname -s)

CC = gcc
CFLAGS := -std=c99 -shared -Wall -fPIC $(shell pkg-config --libs --cflags guile-2.0) $(shell pkg-config sdl2  --cflags --libs)

ifeq ($(UNAME_S),Darwin)
	CFLAGS += -framework OpenGL
endif

libgl_init.so: gl.c
	$(CC) $(CFLAGS) $< -o $@
