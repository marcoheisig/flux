#pragma once
#include <libguile.h>
typedef double real;

#define SCM_TO_REAL(x) scm_to_double(x)
#define SCM_FROM_REAL(x) scm_from_double(x)
