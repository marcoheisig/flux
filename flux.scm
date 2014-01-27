#!/usr/bin/guile \
-e main -s
!#

(add-to-load-path (dirname (current-filename)))
(use-modules
 (ice-9 getopt-long)
 (ice-9 readline)
 (ice-9 format)
 (statprof)
 (sdl2 main)
 (sdl2 video)
 (sdl2 events)
 (opengl gl)
 (system repl error-handling)
 )
(activate-readline)

(define (main args)
  "Flux - the hackable fluid simulator"
  (let* ((option-spec    '((version (single-char #\v) (value #f))
                           (help    (single-char #\h) (value #f))))
         (options        (getopt-long args option-spec))
         (help-wanted    (option-ref options 'help #f))
         (version-wanted (option-ref options 'version #f)))

    (if (or version-wanted help-wanted)
        (begin
          (if version-wanted
              (display "flux xversion 0.1\n"))
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
      (usleep 17))))

