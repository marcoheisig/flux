(define-module (lib jacobi-solver)
  #:export (jacobi-solve
            test-jacobi-solver)
  #:use-module (utils marco)
  #:use-module (lib staggered-grid)
  #:use-module (lib cartesian-grid)
  )

(load-c++ "jacobi_solver.cc" "scm_init_jacobi_solver")

(define (jacobi-solve grid)
  (jacobi-solve-internal 1 2 3 4 5 6 7 8))

(define (test-jacobi-solver xsize ysize itermax eps)
  "perform a simple test with one source and one dest"
  (let ((g1  (make-cartesian-grid "g1"  xsize ysize))
        (g2  (make-cartesian-grid "g2"  xsize ysize))
        (rhs (make-cartesian-grid "rhs" xsize ysize))
        (obs (make-cartesian-grid "obs" xsize ysize))
        (dx      1.0)
        (dy      1.0))
    ; setup the grids
    (cartesian-grid-set! rhs 2 2 10.0)
    (cartesian-grid-set! rhs (- xsize 3) (- ysize 3) -10.0)
    (print-cartesian-grid rhs)
    ; solve & print result
    (format #t "Residual: ~a~%" (jacobi-solve-internal
                                 g1 g2 rhs obs itermax eps dx dy))
    (print-cartesian-grid g1)))

