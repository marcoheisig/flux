(define-module (utils marco)
  #:use-module (system foreign)
  #:use-module (config)
  #:export
  (
   error-here
   define-public-constants
   from-lib
   load-c++
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

(define-syntax load-c++
  (syntax-rules ()
    ((_ file entry-fun lib ...)
     (let* ((loc (assq-ref (current-source-location) 'filename))
            (dir (if (not loc)
                     " "
                      (dirname loc)))
            (filename (string-append dir file-name-separator-string file))
            (libbase  (string-append dir file-name-separator-string "lib" file))
            (libname  (string-append libbase ".so"))
            (build-command (string-append
                            compile-cmd
                            " " libname
                            " " base-flags
                            " " guile-flags
                            " " filename)))
       (display build-command)
       (newline)
       (if (= 0 (system build-command))
           #t
           (error-here 'misc-error 'system
                       (string-append "failed to compile" filename) #f #f))
       (load-extension libbase entry-fun)))))
