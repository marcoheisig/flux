(define-module (lib sor-solver)
  #:export (sor-solve)
  #:use-module (lib staggered-grid))
(chdir "lib")
(if (eqv? 0 (system "make --quiet sor_solver.so"))
    #t
    (error-here 'misc-error 'system "failed to compile libsor_solver.so" #f #f))
(load-extension "./libsor_solver" "scm_init_sor_solver")
(chdir "..")

(define (sor-solve staggered-grid itermax eps)
  5)
