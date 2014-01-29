#include "cartesian_grid.h"

SCM
make_cg(SCM name, SCM scm_xsize, SCM scm_ysize) {
    SCM smob;
    struct cg *cg;
    size_t xsize = scm_to_size_t(scm_xsize);
    size_t ysize = scm_to_size_t(scm_ysize);

    cg = (struct cg*)scm_gc_malloc(sizeof(struct cg),
                                   "cartesian-grid");
    cg->name  = SCM_BOOL_F;
    cg->xsize = xsize;
    cg->ysize = ysize;
    cg->data  = NULL;
    smob      = scm_new_smob(cg_tag, (void*)cg);
    cg->name  = name;
    cg->data  = scm_gc_malloc(xsize * ysize * sizeof(double),
                                  "cartesian-grid data");
    return smob;
}

SCM
clear_cg(SCM cg_smob) {
    struct cg *cg;
    size_t memsize;
    scm_assert_smob_type(cg_tag, cg_smob);

    cg      = (struct cg *) SCM_SMOB_DATA(cg_smob);
    memsize = cg->xsize * cg->ysize * sizeof(real);
    memset(cg->data, 0, memsize);

    return cg_smob;
}

SCM
mark_cg(SCM cg_smob) {
    struct cg *cg = (struct cg *) SCM_SMOB_DATA(cg_smob);
    return cg->name;
}

size_t
free_cg(SCM cg_smob) {
    struct cg *cg = (struct cg *) SCM_SMOB_DATA(cg_smob);
    size_t memsize = cg->xsize * cg->ysize * sizeof(real);
    scm_gc_free(cg->data, memsize, "cartesian-grid data");
    scm_gc_free(cg, sizeof(struct cg), "cartesian-grid");
    return 0;
}

static int
print_cg(SCM cg_smob, SCM port, scm_print_state *pstate) {
    struct cg *cg = (struct cg *) SCM_SMOB_DATA(cg_smob);
    scm_puts("#<cartesian-grid ", port);
    scm_display(cg->name, port);
    scm_puts(">", port);

    scm_remember_upto_here_1(cg_smob);
    return 1;
}

SCM
print_cg_full(SCM cg_smob) {
    scm_assert_smob_type(cg_tag, cg_smob);
    SCM port = scm_current_output_port();

    struct cg *cg = (struct cg *) SCM_SMOB_DATA(cg_smob);
    size_t xsize  = cg->xsize;
    size_t ysize  = cg->ysize;
    real  *data   = cg->data;
    SCM name      = cg->name;
    scm_puts("cartesian-grid: ", port);
    scm_display(name, port);
    scm_puts(" ", port);
    scm_display(scm_from_size_t(xsize), port);
    scm_puts("x", port);
    scm_display(scm_from_size_t(ysize), port);
    scm_newline(port);
    for(int iy = 0; iy < ysize; ++iy) {
        for(int ix = 0; ix < xsize; ++ix) {
            scm_display(scm_from_double(data[index(ix, iy, xsize)]), port);
            scm_puts(" ", port);
        }
        scm_newline(port);
    }
    return cg_smob;
}

SCM
ref_cg(SCM cg_smob, SCM scm_ix, SCM scm_iy) {
    scm_assert_smob_type(cg_tag, cg_smob);
    struct cg *cg = (struct cg *) SCM_SMOB_DATA(cg_smob);
    real    *data = cg->data;
    size_t    xsize = cg->xsize;
    size_t    ysize = cg->ysize;
    size_t ix = scm_to_size_t(scm_ix);
    size_t iy = scm_to_size_t(scm_iy);
    if(ix < 0 || ix >= xsize) scm_out_of_range("ref_cg", scm_ix);
    if(iy < 0 || iy >= ysize) scm_out_of_range("ref_cg", scm_iy);
    scm_remember_upto_here_1(cg_smob);
    return scm_from_double(data[index(ix, iy, xsize)]);
}

SCM
set_cg(SCM cg_smob, SCM scm_ix, SCM scm_iy, SCM scm_value) {
    scm_assert_smob_type(cg_tag, cg_smob);
    struct cg *cg = (struct cg *) SCM_SMOB_DATA(cg_smob);
    real    *data = cg->data;
    size_t    xsize = cg->xsize;
    size_t    ysize = cg->ysize;
    size_t ix = scm_to_size_t(scm_ix);
    size_t iy = scm_to_size_t(scm_iy);
    if(ix < 0 || ix >= xsize) scm_out_of_range("ref_cg", scm_ix);
    if(iy < 0 || iy >= ysize) scm_out_of_range("ref_cg", scm_iy);
    double value = scm_to_double(scm_value);
    data[index(ix, iy, xsize)] = value;
    return cg_smob;
}

void
scm_init_cartesian_grid() {
    cg_tag = scm_make_smob_type("cartesian-grid", sizeof(struct cg));
    scm_set_smob_mark (cg_tag, mark_cg );
    scm_set_smob_free (cg_tag, free_cg );
    scm_set_smob_print(cg_tag, print_cg);

    scm_c_define_gsubr("make-cartesian-grid",  3, 0, 0, make_cg );
    scm_c_define_gsubr("clear-cartesian-grid", 1, 0, 0, clear_cg);
    scm_c_define_gsubr("print-cartesian-grid", 1, 0, 0, print_cg_full);
    scm_c_define_gsubr("cartesian-grid-ref",   3, 0, 0, ref_cg);
    scm_c_define_gsubr("cartesian-grid-set!",  4, 0, 0, set_cg);
}
