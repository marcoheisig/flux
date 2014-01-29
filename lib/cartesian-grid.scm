(define-module (lib cartesian-grid)
  #:export
  (make-cartesian-grid
   clear-cartesian-grid
   print-cartesian-grid
   cartesian-grid-ref
   cartesian-grid-set!))
(chdir "lib")
(system "make --quiet libcartesian_grid.so")
(load-extension "./libcartesian_grid" "scm_init_cartesian_grid")
(chdir "..")
