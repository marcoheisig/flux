(define-module (marco utils)
  #:export
  (
   error-here
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
