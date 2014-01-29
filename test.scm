#!/usr/bin/guile -s
!#

(add-to-load-path (dirname (current-filename)))
(use-modules
 (ice-9 getopt-long)
 (ice-9 readline)
 (ice-9 format)
 (ice-9 top-repl)
 (statprof)
 (sdl2 main)
 (sdl2 video)
 (sdl2 events)
 (opengl gl)
 (lib cartesian-grid)
 (lib staggered-grid)
 (lib sor-solver))

(activate-readline)

(top-repl)
