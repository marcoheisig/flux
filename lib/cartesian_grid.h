#pragma once
#include <libguile.h>
#include "config.h"

struct cg {
    SCM name;
    size_t xsize;
    size_t ysize;
    real *data;

    const real& operator() (size_t ix, size_t iy) const
    { return data[iy * xsize + ix]; }

    real& operator() (size_t ix, size_t iy)
    { return data[iy * xsize + ix]; }
};
