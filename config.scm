; one day this module should be auto-generated by a configure script
(define-module (config))

(define-public compile-cmd  "g++ -o")
(define-public base-flags   "-Ofast -std=c++0x -shared -Wall -fPIC")
(define-public guile-flags  "-pthread -I/usr/include/guile/2.0 -lguile-2.0 -lgc")
(define-public sdl2-flags   "-D_REENTRANT -I/usr/include/SDL2 -lSDL2 -lpthread")
(define-public opengl-flags "-I/usr/include/libdrm -lGL")

