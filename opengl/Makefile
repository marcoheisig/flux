CC = gcc
CFLAGS := -std=c99 -shared -Wall -fPIC $(shell pkg-config --libs --cflags guile-2.0)

libgl_init.so: gl.c
	$(CC) $(CFLAGS) $< -o $@
