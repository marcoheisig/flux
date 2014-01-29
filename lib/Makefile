CC = gcc
CFLAGS := -std=c99 -shared -Wall -lguile -fPIC

all: libcartesian_grid.so sor_solver.so

libcartesian_grid.so: cartesian_grid.c cartesian_grid.h
	$(CC) $(CFLAGS) $< -o $@

sor_solver.so: sor_solver.c cartesian_grid.h
	$(CC) $(CFLAGS) $< -o $@
