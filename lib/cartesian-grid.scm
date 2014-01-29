(define-module (lib cartesian-grid)
  #:use-module (utils marco)
  #:export
  (make-cartesian-grid
   clear-cartesian-grid
   print-cartesian-grid
   cartesian-grid-ref
   cartesian-grid-set!))
(chdir "lib")
(if (eqv? 0 (system "make --quiet libcartesian_grid.so"))
    #t
    (error-here 'misc-error 'system "failed to compile libcartesian_grid.so" #f #f))
(load-extension "./libcartesian_grid" "scm_init_cartesian_grid")
(chdir "..")
