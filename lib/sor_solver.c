#include "cartesian_grid.h"

SCM sor_solve_internal(SCM scm_grid1, SCM scm_grid2,
                       SCM scm_rhs, SCM scm_itermax, SCM scm_eps) {
    scm_assert_smob_type(cg_tag, scm_grid1);
    scm_assert_smob_type(cg_tag, scm_grid2);
    scm_assert_smob_type(cg_tag, scm_rhs);

    size_t itermax  = scm_to_size_t(scm_itermax);
    double eps      = scm_to_double(scm_eps);
    struct cg *src  = (struct cg *) SCM_SMOB_DATA(scm_grid1);
    struct cg *dest = (struct cg *) SCM_SMOB_DATA(scm_grid2);
    struct cg *rhs  = (struct cg *) SCM_SMOB_DATA(scm_rhs);

    SCM_ASSERT_TYPE(src->xsize != rhs->xsize ||
                    src->ysize != rhs->ysize,
                    scm_rhs,
                    SCM_ARGn,
                    "sor_solve",
                    "grids and rhs must have same dimensions");

    size_t xsize     = src->xsize;
    size_t ysize     = src->ysize;
    real  *g         = src->data;
    real  *r         = rhs->data;
    SCM name         = src->name;

    for(int iy = 0; iy < ysize; ++iy) {
        for(int ix = 0; ix < xsize; ++ix) {
            g[index(ix, iy, xsize)];
        }
    }

    scm_remember_upto_here_1(scm_rhs);
    scm_remember_upto_here_1(scm_grid2);
    return scm_grid1;
}

void
scm_init_sor_solver() {
    scm_c_define_gsubr("sor-solve-internal", 4, 0, 0, sor_solve_internal);
}
