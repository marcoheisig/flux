(define-module (sdl2 main)
  #:use-module (system foreign)
  #:use-module (sdl2 error)
  #:use-module (marco libwrap)
  #:export
  (
   SDL_INIT_TIMER
   SDL_INIT_AUDIO
   SDL_INIT_VIDEO
   SDL_INIT_JOYSTICK
   SDL_INIT_HAPTIC
   SDL_INIT_GAMECONTROLLER
   SDL_INIT_EVENTS
   SDL_INIT_NOPARACHUTE
   SDL_INIT_EVERYTHING
   init
   init-subsystem
   quit-all
   quit-subsystem
   set-main-ready
   was-init))

(define SDL_INIT_TIMER          #x00000001)
(define SDL_INIT_AUDIO          #x00000010)
(define SDL_INIT_VIDEO          #x00000020)
(define SDL_INIT_JOYSTICK       #x00000200)
(define SDL_INIT_HAPTIC         #x00001000)
(define SDL_INIT_GAMECONTROLLER #x00002000)
(define SDL_INIT_EVENTS         #x00004000)
(define SDL_INIT_NOPARACHUTE    #x00100000)
(define SDL_INIT_EVERYTHING     (logior
                                 SDL_INIT_TIMER
                                 SDL_INIT_AUDIO
                                 SDL_INIT_VIDEO
                                 SDL_INIT_JOYSTICK
                                 SDL_INIT_HAPTIC
                                 SDL_INIT_GAMECONTROLLER
                                 SDL_INIT_EVENTS))

(define libsdl2 (dynamic-link "libSDL2"))
(from-lib
 libsdl2
 (SDL_Init          int    "SDL_Init"          uint32)
 (SDL_InitSubSystem int    "SDL_InitSubSystem" uint32)
 (SDL_Quit          void   "SDL_Quit"                )
 (SDL_QuitSubSystem void   "SDL_QuitSubSystem" uint32)
 (SDL_SetMainReady  void   "SDL_SetMainReady"        )
 (SDL_WasInit       uint32 "SDL_WasInit"       uint32)
 )

(define (init flags)
  "Initialize the SDL library. This must be called before using any other SDL function."
  (if (= 0 (SDL_Init flags))
      flags
      (error (get-error))))

(define (init-subsystem flags)
  "Initialize specific SDL subsystems."
  (if (= 0 (SDL_InitSubSystem flags))
      flags
      (error (get-error))))

(define (quit-all)
  "Clean up all initialized subsystems"
  (SDL_Quit))

(define (quit-subsystem flags)
  "Clean up specific SDL subsystems."
  (SDL_QuitSubSystem flags))

(define (set-main-ready)
  "Call to circumvent failure of SDL_Init() when not using SDL_main() as an entry point."
  (SDL_SetMainReady))

(define (was-init flags)
  "Returns a mask of the specified subsystems."
  (SDL_WasInit flags))
