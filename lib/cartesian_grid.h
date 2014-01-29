#pragma once
#include <libguile.h>
#include "config.h"

static scm_t_bits cg_tag;
struct cg {
    SCM name;
    size_t xsize;
    size_t ysize;
    real *data;
};

static inline size_t
idx(size_t ix, size_t iy, size_t stride) {
    return iy * stride + ix;
}
