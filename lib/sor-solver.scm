(define-module (lib sor-solver)
  #:export (sor-solve)
  #:use-module (lib staggered-grid))

(load-extension "./lib/sor_solver" "scm_init_sor_solver")
(define (sor-solve staggered-grid itermax eps)
  5)
