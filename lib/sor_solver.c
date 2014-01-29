#include "cartesian_grid.h"

SCM sor_solve(SCM scm_grid1, SCM scm_grid2, SCM scm_rhs,
              SCM scm_itermax, SCM scm_eps,
              SCM scm_dx, SCM scm_dy, SCM scm_omega) {
    scm_assert_smob_type(cg_tag, scm_grid1);
    scm_assert_smob_type(cg_tag, scm_grid2);
    scm_assert_smob_type(cg_tag, scm_rhs);

    size_t itermax  = scm_to_size_t(scm_itermax);
    double eps      = scm_to_double(scm_eps);
    double dx       = scm_to_double(scm_dx);
    double dy       = scm_to_double(scm_dy);
    double omega    = scm_to_double(scm_omega);

    struct cg *src  = (struct cg *) SCM_SMOB_DATA(scm_grid1);
    struct cg *dest = (struct cg *) SCM_SMOB_DATA(scm_grid2);
    struct cg *rhs  = (struct cg *) SCM_SMOB_DATA(scm_rhs);

    SCM_ASSERT_TYPE(src->xsize != dest->xsize ||
                    src->ysize != dest->ysize ||
                    src->xsize != dest->xsize ||
                    src->ysize != dest->ysize,
                    scm_rhs,
                    SCM_ARGn,
                    "sor_solve",
                    "grids and rhs must have same size");

    size_t xsize     = src->xsize;
    size_t ysize     = src->ysize;
    real  *s         = src->data;
    real  *d         = dest->data;
    real  *r         = rhs->data;
    SCM name         = src->name;

    /* here comes the actual solver */
    for(int i = 0; i < itermax; ++i) {
        /* one iteration */

        /* foo */
    }
    /* end of solver, finalize */
    scm_remember_upto_here_1(scm_rhs);
    scm_remember_upto_here_1(scm_grid2);
    return scm_grid1;
}

void
scm_init_sor_solver() {
    scm_c_define_gsubr("sor-solve-internal", 4, 0, 0, sor_solve);
}
