(define-module (marco utils)
  #:use-module (system foreign)
  #:export
  (
   error-here
   define-public-constants
   from-lib
   ))

(define-syntax error-here
  (syntax-rules ()
    ((_ key subr msg args data)
     (let* ((alist (current-source-location))
            (filename                 (assq-ref alist 'filename))
            (line     (number->string (assq-ref alist 'line)))
            (column   (number->string (assq-ref alist 'column)))
            (message (string-concatenate
                      (list "\nfile: "     filename
                            "\nline: "    line
                            "\ncolumn: "  column
                            "\nmessage: " msg))))
       (scm-error key subr message args data)))))

(define-syntax define-public-constants
  (syntax-rules ()
    ((_ (symbol value))
     (define-public symbol value))
    ((_ (symbol value) c1 c2 ...)
     (begin (define-public-constants (symbol value))
            (define-public-constants c1 c2 ...)))))

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
