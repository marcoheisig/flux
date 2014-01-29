CC = gcc
CFLAGS := -std=c99 -shared -Wall -fPIC $(shell pkg-config --libs --cflags guile-2.0)
LIBS := libcartesian_grid.so sor_solver.so

all: $(LIBS)

libcartesian_grid.so: cartesian_grid.c cartesian_grid.h
	$(CC) $(CFLAGS) $< -o $@

libsor_solver.so: sor_solver.c cartesian_grid.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm $(LIBS)
