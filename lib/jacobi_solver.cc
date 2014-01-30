#include <utility>
#include <cmath>
#include "cartesian_grid.h"
using namespace std;

real residual_norm(const cg& g, const cg& rhs, const cg& obs,
                   real dx, real dy) {
    size_t counter = 0;
    real   acc     = 0.0;
    size_t xsize = g.xsize;
    size_t ysize = g.ysize;
    const real dx2inv = 1.0 / (dx * dx);
    const real dy2inv = 1.0 / (dy * dy);

    for(size_t iy  = 1; iy < ysize - 1; ++iy) {
        for(size_t ix  = 1; ix < xsize - 1; ++ix) {
            //if(0.0 != obs(ix, iy)) continue; // skip obstacles
            real r = rhs(ix, iy) -
                ((g(ix+1, iy) - 2.0 * g(ix, iy) + g(ix-1, iy)) * dx2inv) -
                ((g(ix, iy+1) - 2.0 * g(ix, iy) + g(ix, iy-1)) * dy2inv);
            ++counter;
            acc += r * r;
        }
    }
    return sqrt(acc / (real)counter);
}

SCM jacobi_solve(SCM scm_grid1, SCM scm_grid2, SCM scm_rhs, SCM scm_obs,
              SCM scm_itermax, SCM scm_eps,
              SCM scm_dx, SCM scm_dy) {
    SCM_ASSERT_TYPE(scm_to_bool(scm_call_1(scm_c_public_ref("lib cartesian-grid",
                                                            "cartesian-grid?"),
                                           scm_grid1)),
                    scm_grid1,
                    SCM_ARG1,
                    "jacobi_solve",
                    "cartesian-grid");
    // scm_assert_smob_type(tag, scm_grid1);
    // scm_assert_smob_type(tag, scm_grid2);
    // scm_assert_smob_type(tag, scm_rhs);
    // scm_assert_smob_type(tag, scm_obs);

    size_t itermax  = scm_to_size_t(scm_itermax);
    real eps        = SCM_TO_REAL(scm_eps);
    real dx         = SCM_TO_REAL(scm_dx);
    real dy         = SCM_TO_REAL(scm_dy);

    cg &g1   = *(struct cg *) SCM_SMOB_DATA(scm_grid1);
    cg &g2   = *(struct cg *) SCM_SMOB_DATA(scm_grid2);
    cg &rhs  = *(struct cg *) SCM_SMOB_DATA(scm_rhs);
    cg &obs  = *(struct cg *) SCM_SMOB_DATA(scm_rhs);

    SCM_ASSERT_TYPE(g1.xsize == g2.xsize   &&
                    g1.ysize == g2.ysize   &&
                    g1.xsize == obs.xsize  &&
                    g1.ysize == obs.ysize  &&
                    g1.xsize == rhs.xsize  &&
                    g1.ysize == rhs.ysize,
                    scm_grid1,
                    SCM_ARGn,
                    "jacobi_solve",
                    "grids, rhs and obs of the same size");

    size_t xsize     = g1.xsize;
    size_t ysize     = g1.ysize;

    /* here comes the actual solver */
    real residual = 0.0;
    cg *src_ptr  = (struct cg *) SCM_SMOB_DATA(scm_grid1);
    cg *dest_ptr = (struct cg *) SCM_SMOB_DATA(scm_grid2);
    for(size_t i = 0; i < itermax; ++i) {
        cg &src  = *src_ptr;
        cg &dest = *dest_ptr;

        residual = residual_norm(src, rhs, obs, dx, dy);
        if(residual <= eps) break;

        /* Neumann boundaries */
        for(size_t ix = 1; ix < xsize-1; ++ix) {
            dest(ix, 0)       = dest(ix, 1);
            dest(ix, ysize-1) = dest(ix, ysize-2);
        }
        for(size_t iy = 1; iy < ysize-1; ++iy) {
            dest(0, iy)       = dest(1, iy);
            dest(xsize-1, iy) = dest(xsize-2, iy);
        }

        /* one iteration */
        real dx2inv = 1.0 / (dx * dx);
        real dy2inv = 1.0 / (dy * dy);
        real winv = 1.0 / (2.0 * dx2inv + 2.0 * dy2inv);
        for(size_t iy  = 1; iy < ysize - 1; ++iy) {
            for(size_t ix  = 1; ix < xsize - 1; ++ix) {
                //if(0.0 != obs(ix, iy)) continue; // skip obstacles
                dest(ix, iy) = winv *
                    (((src(ix+1, iy) + src(ix-1, iy)) * dx2inv) +
                     ((src(ix, iy+1) + src(ix, iy-1)) * dy2inv) - rhs(ix, iy));
            }
        }
        swap(src_ptr, dest_ptr);
    }
    /* end of solver, finalize */
    scm_remember_upto_here_1(scm_rhs);
    scm_remember_upto_here_1(scm_grid1);
    scm_remember_upto_here_1(scm_grid2);
    scm_remember_upto_here_1(scm_obs);
    return SCM_FROM_REAL(residual);
}

extern "C" {
    void
    scm_init_jacobi_solver() {
        scm_c_define_gsubr("jacobi-solve-internal", 8, 0, 0, (void*)jacobi_solve);
    }
}
