(define-module (flux cartesian-grid)
  #:export
  (make-cartesian-grid
   clear-cartesian-grid
   print-cartesian-grid
   cartesian-grid-ref
   cartesian-grid-set!))

(load-extension "./flux/libcartesian_grid" "scm_init_cartesian_grid")
