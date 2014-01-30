(define-module (lib cartesian-grid)
  #:use-module (utils marco)
  #:export
  (make-cartesian-grid
   clear-cartesian-grid
   print-cartesian-grid
   cartesian-grid-ref
   cartesian-grid-set!
   cartesian-grid?
   ))

(load-c++ "cartesian_grid.cc" "scm_init_cartesian_grid")
