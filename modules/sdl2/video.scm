(define-module (sdl2 video)
  #:use-module (system foreign)
  #:use-module (rnrs bytevectors)
  #:use-module (sdl2 error)
  #:use-module (marco libwrap)
  #:use-module (marco utils)
  #:export
  (
   SDL_WINDOW_FULLSCREEN
   SDL_WINDOW_OPENGL
   SDL_WINDOW_SHOWN
   SDL_WINDOW_HIDDEN
   SDL_WINDOW_BORDERLESS
   SDL_WINDOW_RESIZABLE
   SDL_WINDOW_MINIMIZED
   SDL_WINDOW_MAXIMIZED
   SDL_WINDOW_INPUT_GRABBED
   SDL_WINDOW_INPUT_FOCUS
   SDL_WINDOW_MOUSE_FOCUS
   SDL_WINDOW_FULLSCREEN_DESKTOP
   SDL_WINDOW_FOREIGN
   SDL_WINDOW_ALLOW_HIGHDPI
   create-window
   create-window-and-renderer
   create-window-from
   destroy-window
   ))

(define SDL_WINDOW_FULLSCREEN          #x00000001)
(define SDL_WINDOW_OPENGL              #x00000002)
(define SDL_WINDOW_SHOWN               #x00000004)
(define SDL_WINDOW_HIDDEN              #x00000008)
(define SDL_WINDOW_BORDERLESS          #x00000010)
(define SDL_WINDOW_RESIZABLE           #x00000020)
(define SDL_WINDOW_MINIMIZED           #x00000040)
(define SDL_WINDOW_MAXIMIZED           #x00000080)
(define SDL_WINDOW_INPUT_GRABBED       #x00000100)
(define SDL_WINDOW_INPUT_FOCUS         #x00000200)
(define SDL_WINDOW_MOUSE_FOCUS         #x00000400)
(define SDL_WINDOW_FULLSCREEN_DESKTOP (logior SDL_WINDOW_FULLSCREEN #x00001000))
(define SDL_WINDOW_FOREIGN             #x00000800)
(define SDL_WINDOW_ALLOW_HIGHDPI       #x00002000)

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
 (SDL_GL_ExtensionSupported    int "SDL_GL_ExtensionSupported"   '*                       )
 (SDL_GL_GetNumVideoDrivers    int "SDL_GetNumVideoDrivers"                               )
 (SDL_GetVideoDriver            '* "SDL_GetVideoDriver"          int                      )
 )

(define (create-window title x y w h flags)
  "Create a SDL window with the specified position, dimensions and flags"
  (let ((*window (SDL_CreateWindow (string->pointer title "utf8") x y w h flags)))
    (if (null-pointer? *window)
        (error (get-error) (current-source-location))
        *window)))

(define (destroy-window *window)
  "Destroy a SDL window"
  (if (not (pointer? *window))
      (error "pointer expected" (current-source-location))
      (SDL_DestroyWindow *window)))

(define (create-window-and-renderer width height flags)
  "Create a SDL window and associated renderer"
  (let* (( *window   (make-bytevector (sizeof '*)))
         (**window   (bytevector->pointer *window))
         ( *renderer (make-bytevector (sizeof '*)))
         (**renderer (bytevector->pointer *renderer)))
    (if (= 0 (SDL_CreateWindowAndRenderer width height flags **window **renderer))
        (values (dereference-pointer **window)
                (dereference-pointer **renderer))
        (error (get-error) (current-source-location)))))
