(define-module (marco libwrap)
  #:use-module (system foreign)
  #:export (from-lib))

;; my first scheme macro ever
;; TOOD do some checking and maybe evaluate "lib" first
(define-syntax from-lib
  (syntax-rules ()
    ((_ lib (symbol rtn name arg ...))
     (define symbol (pointer->procedure
                     rtn
                     (dynamic-func name lib)
                     (list arg ...))))
    ((_ lib (symbol rtn name arg ...) c1 c2 ...)
     (begin (from-lib lib (symbol rtn name arg ...))
            (from-lib lib c1 c2 ...)))))
