(define name (make-parameter "Lid Driven Cavity"))

(define N-boundary (make-parameter '(noslip . 1.0)))
(define S-boundary (make-parameter '(noslip . 0.0)))
(define W-boundary (make-parameter '(noslip . 0.0)))
(define E-boundary (make-parameter '(noslip . 0.0)))

; Reynolds number
(define Re         (make-parameter 100.0))

; gravity
(define gx         (make-parameter 0.0))
(define gy         (make-parameter 0.0))

(define u-init     (make-parameter 0.0))
(define v-init     (make-parameter 0.0))
(define p-init     (make-parameter 0.0))

; define the rectangular domain
(define x-size     (make-parameter 1.0))
(define y-size     (make-parameter 1.0))
(define x-gridsize (make-parameter 30))
(define y-gridsize (make-parameter 30))

; timesetpping
(define dt         (make-parameter 0.02))
(define timesteps  (make-parameter 0.02))
(define tau        (make-parameter 0.02)) ; factor for timestep size control (<=0 means constant dt)

; pressure iteration data
(define itermax    (make-parameter 1000))   ; maximal number of pressure iteration in one time step
(define eps        (make-parameter 0.0001)) ; stopping tolerance for pressure iteration
(define omg        (make-parameter 1.8))    ; relaxation parameter for SOR iteration
(define gamma      (make-parameter 0.9))    ; upwind differencing factor gamma
(define checkfreq  (make-parameter 5))      ; every n'th SOR iteration the residuum is calculated and checked against eps
(define normfreq   (make-parameter 20))     ; every n'th time step in the fluid solver the pressure is centered around 0

(define vis-ration (make-parameter 5))      ; visualize every nth timestep
