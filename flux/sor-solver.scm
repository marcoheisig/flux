(define-module (flux sor-solver)
  #:export
  (sor-solve))

(load-extension "./flux/sor_solver" "scm_init_sor_solver")
