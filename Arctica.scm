#!/usr/bin/guile \
-e main -s
!#
(add-to-load-path "modules")
(use-modules (ice-9 getopt-long)
             (ice-9 readline)
             (sdl2 main)
             (sdl2 video)
             (system repl error-handling)
             (ice-9 format))

(activate-readline)

(define (main args)
  "Arctica - the awesome fluid simulator"
  (let* ((option-spec '((version (single-char #\v) (value #f))
                        (help    (single-char #\h) (value #f))))
         (options (getopt-long args option-spec))
         (help-wanted (option-ref options 'help #f))
         (version-wanted (option-ref options 'version #f)))

    (if (or version-wanted help-wanted)
        (begin
          (if version-wanted
              (display "Arctica version 0.1\n"))
          (if help-wanted
              (display
               "Arctica [options]\n")))
        (call-with-error-handling
           (lambda ()(function))))))

(define (function)
  (error "Nothing is implemented yet!"))
