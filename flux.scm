#!guile \ 
-e main -s
!#
(define flux-version "0.1 alpha")

(add-to-load-path (getenv "PWD"))
(use-modules
 (ice-9 getopt-long)
 (ice-9 readline)
 (ice-9 format)
 (statprof)
 (sdl2 main)
 (sdl2 video)
 (sdl2 events)
 (opengl gl)
 (lib cartesian-grid)
 (lib staggered-grid)
 (system repl error-handling))
(activate-readline)
(load "params.scm")

(define (main args)
  "Flux - the hackable fluid simulator"
  (let* ((option-spec    '((version (single-char #\v) (value #f))
                           (help    (single-char #\h) (value #f))))
         (options        (getopt-long args option-spec))
         (help-wanted    (option-ref options 'help #f))
         (version-wanted (option-ref options 'version #f)))
    (format #t "Flux v~a - flux is free software under the GPLv3~%" flux-version)
    (if (or version-wanted help-wanted)
        (begin
          (if version-wanted
              (format #t "flux v~a\n" flux-version))
          (if help-wanted
              (display
               "flux [options]\n")))
        (call-with-error-handling
         (lambda ()(main-fun))))))

(define (main-fun)
  (statprof-reset 0 50000 #t)
  (statprof-start)
  (init SDL_INIT_EVERYTHING)
  (add-hook! quit-event-hook
             (lambda (type timestamp)
               (quit-all)
               (statprof-stop)
               (statprof-display)
                (quit)))
  (add-hook! window-event-hook
             (lambda (type timestamp window-id event data1 data2)
               (format #t "~s:WINDOW      window ~s event ~s data1 ~ſ data2 ~s~%"
                       timestamp window-id event data1 data2)))
  (add-hook! mouse-button-event-hook
             (lambda (type timestamp window-id which button state x y)
               (format #t "~s:MOUSEBUTTON window ~s which ~s button ~s state ~s x ~s y ~s~%"
                       timestamp window-id which button state x y)))
  (add-hook! keyboard-event-hook
             (lambda (type timestamp window-id state repeat scancode keycode mod)
               (format #t "~s:KEYBOARD    window ~s state ~s repeat ~s scancode ~s keycode ~s mod ~s~%"
                       timestamp window-id state repeat scancode keycode mod)))
  (add-hook! mousemotion-event-hook
             (lambda (type timestamp window-id which state x y xrel yrel)
               (format #t "~s:MOUSEMOTION window ~s which ~s state ~s x ~s y ~s xrel ~s yrel ~s~%"
                       timestamp window-id which state x y xrel yrel)))
  (let* ((*window  (create-window "Flux Fluid Simulator"
                                 5 5 400 400 (logior SDL_WINDOW_SHOWN
                                                     SDL_WINDOW_OPENGL)))
         (*context (gl-create-context *window))
         (color    0.0)
         (up       #t))
    (init-opengl)
    (while #t
      (handle-events)
      (clear-color color color color 0.0)
      (clear GL_COLOR_BUFFER_BIT)
      (gl-swap-window *window)
      (if (>= color 1.0)
          (set! up #f))
      (if (<= color 0.0)
          (set! up #t))
      (if up
          (set! color (+ color 0.01))
          (set! color (- color 0.01)))
      (usleep 17000))))

