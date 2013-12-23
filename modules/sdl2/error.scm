(define-module (sdl2 error)
  #:use-module (system foreign)
  #:use-module (marco libwrap)
  #:export (clear-error
            get-error
            set-error))

(define libsdl2 (dynamic-link "libSDL2"))
(from-lib
 libsdl2
 (SDL_ClearError void "SDL_ClearError"   )
 (SDL_GetError   '*   "SDL_GetError"     )
 (SDL_SetError   int  "SDL_SetError"   '*))

(define (clear-error)
  "Clear any previous error message."
  (SDL_ClearError))

(define (get-error)
  "Returns a message about the last error that has occured, or an empty string
 if there hasn't been an error since the last call to clear-error."
  (pointer->string (SDL_GetError) -1 "utf8"))

(define (set-error string)
  "This function sets the SDL error string."
  (SDL_SetError (string->pointer string "utf8"))
  string)
