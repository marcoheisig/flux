(define-module (sdl2 video)
  #:use-module (system foreign)
  #:use-module (rnrs bytevectors)
  #:use-module (sdl2 error)
  #:use-module (marco utils)
  #:export
  (
   create-window
   create-window-and-renderer
   destroy-window
   gl-create-context
   gl-delete-context
   gl-swap-window
   get-video-drivers
   ))

(define-public-constants
  (SDL_WINDOW_FULLSCREEN          #x00000001)
  (SDL_WINDOW_OPENGL              #x00000002)
  (SDL_WINDOW_SHOWN               #x00000004)
  (SDL_WINDOW_HIDDEN              #x00000008)
  (SDL_WINDOW_BORDERLESS          #x00000010)
  (SDL_WINDOW_RESIZABLE           #x00000020)
  (SDL_WINDOW_MINIMIZED           #x00000040)
  (SDL_WINDOW_MAXIMIZED           #x00000080)
  (SDL_WINDOW_INPUT_GRABBED       #x00000100)
  (SDL_WINDOW_INPUT_FOCUS         #x00000200)
  (SDL_WINDOW_MOUSE_FOCUS         #x00000400)
  (SDL_WINDOW_FULLSCREEN_DESKTOP (logior SDL_WINDOW_FULLSCREEN #x00001000))
  (SDL_WINDOW_FOREIGN             #x00000800)
  (SDL_WINDOW_ALLOW_HIGHDPI       #x00002000))

(define libsdl2 (dynamic-link "libSDL2"))

(from-lib
 libsdl2
 (SDL_CreateWindow              '* "SDL_CreateWindow"            '* int int int int uint32)
 (SDL_CreateWindowAndRenderer  int "SDL_CreateWindowAndRenderer" int int uint32 '* '*     )
 (SDL_CreateWindowFrom          '* "SDL_CreateWindowFrom"        '*                       )
 (SDL_DestroyWindow           void "SDL_DestroyWindow"           '*                       )
 (SDL_DisableScreenSaver      void "SDL_DisableScreenSaver"                               )
 (SDL_EnableScreenSaver       void "SDL_EnableScreenSaver"                                )
 (SDL_GL_BindTexture           int "SDL_GL_BindTexture"          '* '* '*                 )
 (SDL_GL_CreateContext          '* "SDL_GL_CreateContext"        '*                       )
 (SDL_GL_DeleteContext        void "SDL_GL_DeleteContext"        '*                       )
 (SDL_GL_SwapWindow           void "SDL_GL_SwapWindow"           '*                       )
 (SDL_GL_ExtensionSupported    int "SDL_GL_ExtensionSupported"   '*                       )
 (SDL_GetNumVideoDrivers       int "SDL_GetNumVideoDrivers"                               )
 (SDL_GetVideoDriver            '* "SDL_GetVideoDriver"          int                      )
 )

(define (create-window title x y w h flags)
  "Create a SDL window with the specified position, dimensions and flags"
  (let ((*window (SDL_CreateWindow (string->pointer title "utf8") x y w h flags)))
    (if (null-pointer? *window)
        (error-here 'misc-error 'create-window (get-error) #f #f)
        *window)))

(define (create-window-and-renderer width height flags)
  "Create a SDL window and associated renderer"
  (let* (( *window   (make-bytevector (sizeof '*)))
         (**window   (bytevector->pointer *window))
         ( *renderer (make-bytevector (sizeof '*)))
         (**renderer (bytevector->pointer *renderer)))
    (if (= 0 (SDL_CreateWindowAndRenderer width height flags **window **renderer))
        (values (dereference-pointer **window)
                (dereference-pointer **renderer))
        (error-here 'misc-error 'create-window-and-renderer (get-error) #f #f))))

(define (gl-create-context *window)
  "Returns the OpenGL context associated with window"
  (let ((*context (SDL_GL_CreateContext *window)))
    (if (null-pointer? *context)
        (error-here 'misc-error 'gl-create-context (get-error) #f #f)
        *context)))

(define (gl-delete-context *context)
  "Delete a given OpenGL context"
  (SDL_GL_DeleteContext *context))

(define (destroy-window *window)
  "Destroy a SDL window"
  (if (not (pointer? *window))
      (error-here 'wrong-type-arg 'destroy-window "pointer expected" #f #f)
      (SDL_DestroyWindow *window)))

(define (gl-swap-window *window)
  "Swap the GL buffer of the given window"
  (SDL_GL_SwapWindow *window))

(define (get-video-drivers)
  "Get a list of all avilable video drivers"
  (let ((num-drivers (SDL_GetNumVideoDrivers)))
    (if (negative? num-drivers)
        (error-here 'misc-error 'get-video-drivers (get-error) #f #f)
        (map (lambda (x) (pointer->string x -1 "utf8"))
             (map (lambda (x) (SDL_GetVideoDriver x))
                  (iota num-drivers))))))
