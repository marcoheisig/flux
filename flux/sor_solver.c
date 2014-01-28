#include "cartesian_grid.h"

SCM sor_solve(SCM scm_grid, SCM scm_rhs, SCM scm_itermax, SCM scm_eps) {
    scm_assert_smob_type(cg_tag, scm_grid);
    scm_assert_smob_type(cg_tag, scm_rhs);

    size_t itermax  = scm_to_size_t(scm_itermax);
    double eps      = scm_to_double(scm_eps);
    struct cg *grid = (struct cg *) SCM_SMOB_DATA(scm_grid);
    struct cg *rhs  = (struct cg *) SCM_SMOB_DATA(scm_rhs);

    SCM_ASSERT_TYPE(grid->xsize != rhs->xsize ||
                    grid->ysize != rhs->ysize,
                    rhs,
                    SCM_ARGn,
                    "sor_solve",
                    "grid and rhs must have same dimensions");

    size_t xsize     = grid->xsize;
    size_t ysize     = grid->ysize;
    real  *g         = grid->data;
    real  *r         = rhs->data;
    SCM name         = grid->name;

    for(int iy = 0; iy < ysize; ++iy) {
        for(int ix = 0; ix < xsize; ++ix) {
            g[index(ix, iy, xsize)]
        }
    }

    scm_remember_upto_here_1(rhs);
    return rhs;
}

void
scm_init_sor_solver() {
    scm_c_define_gsubr("sor-solve", 4, 0, 0, sor_solve );
}
