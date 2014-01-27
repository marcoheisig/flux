(define-module (sdl2 render)
  #:use-module (sdl2 error)
  #:use-module (utils marco)
  #:use-module (system foreign)
  #:export
  (
   create-renderer
   ))

(define-public-constants
  (SDL_RENDERER_SOFTWARE       #x00000001)
  (SDL_RENDERER_ACCELERATED    #x00000002)
  (SDL_RENDERER_PRESENTVSYNC   #x00000004)
  (SDL_RENDERER_TARGETTEXTURE  #x00000008)
  (SDL_TEXTUREACCESS_STATIC    #x00000000)
  (SDL_TEXTUREACCESS_STREAMING #x00000001)
  (SDL_TEXTUREACCESS_TARGET    #x00000002)
  (SDL_TEXTUREMODULATE_NONE    #x00000000)
  (SDL_TEXTUREMODULATE_COLOR   #x00000001)
  (SDL_TEXTUREMODULATE_ALPHA   #x00000002)
  (SDL_FLIP_NONE               #x00000000)
  (SDL_FLIP_HORIZONTAL         #x00000001)
  (SDL_FLIP_VERTICAL           #x00000002))

(define libsdl2 (dynamic-link "libSDL2"))
(from-lib
 libsdl2
  (SDL_CreateRenderer '* "SDL_CreateRenderer" '* int uint32)
  )

(define (create-renderer *window index flags)
  "Create a renderer for the driver specified by index"
  (if (not (pointer? *window))
      (error-here 'wrong-type-arg 'create-renderer "pointer expected" #f #f)
      (let ((*renderer (SDL_CreateRenderer *window index flags)))
        (if (null-pointer? *renderer)
            (error-here 'misc-error 'create-renderer (get-error) #f #f)
            *renderer))))
