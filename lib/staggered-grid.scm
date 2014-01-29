(define-module (lib staggered-grid)
  #:use-module (lib cartesian-grid)
  #:use-module (oop goops)
  #:export
  (make-staggered-grid
   pressure
   rhs
   u
   v
   f
   g
   o
   ))

(define-class <staggered-grid> ()
  (xsize #:init-keyword #:xsize)
  (ysize #:init-keyword #:ysize)
  (p #:init-keyword #:p #:accessor pressure)
  (q #:init-keyword #:p #:accessor pressure_tmp) ; jacobi needs two pressure arrays
  (r #:init-keyword #:r #:accessor rhs)
  (u #:init-keyword #:u #:accessor u)
  (v #:init-keyword #:v #:accessor v)
  (f #:init-keyword #:f #:accessor f)
  (g #:init-keyword #:g #:accessor g)
  (o #:init-keyword #:o #:accessor obstacle))

(define (make-staggered-grid xsize ysize)
  (make <staggered-grid>
    #:p (make-cartesian-grid "pressure"   xsize      ysize)
    #:q (make-cartesian-grid "pressure"   xsize      ysize)
    #:r (make-cartesian-grid "rhs"        xsize      ysize)
    #:u (make-cartesian-grid "x velocity" (1- xsize) ysize)
    #:v (make-cartesian-grid "y velocity" xsize      (1- ysize))
    #:f (make-cartesian-grid "x terms"    (1- xsize) ysize)
    #:g (make-cartesian-grid "y terms"    xsize      (1- ysize))
    #:o (make-cartesian-grid "obstacles"  xsize      ysize)))

(define-public (print-staggered-grid grid)
  (print-cartesian-grid (pressure grid))
  (print-cartesian-grid (rhs grid))
  (print-cartesian-grid (u grid))
  (print-cartesian-grid (v grid))
  (print-cartesian-grid (f grid))
  (print-cartesian-grid (g grid))
  (print-cartesian-grid (obstacle grid)))
