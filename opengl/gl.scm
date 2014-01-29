(define-module (opengl gl)
  #:use-module (system foreign)
  #:use-module (rnrs bytevectors)
  #:use-module (utils marco)
  #:export
  (
   clear-color
   clear
   init-opengl
   ))

(define-public-constants
  (GL_CURRENT_BIT         #x00000001)
  (GL_POINT_BIT           #x00000002)
  (GL_LINE_BIT            #x00000004)
  (GL_POLYGON_BIT         #x00000008)
  (GL_POLYGON_STIPPLE_BIT #x00000010)
  (GL_PIXEL_MODE_BIT      #x00000020)
  (GL_LIGHTING_BIT        #x00000040)
  (GL_FOG_BIT             #x00000080)
  (GL_DEPTH_BUFFER_BIT    #x00000100)
  (GL_ACCUM_BUFFER_BIT    #x00000200)
  (GL_STENCIL_BUFFER_BIT  #x00000400)
  (GL_VIEWPORT_BIT        #x00000800)
  (GL_TRANSFORM_BIT       #x00001000)
  (GL_ENABLE_BIT          #x00002000)
  (GL_COLOR_BUFFER_BIT    #x00004000)
  (GL_HINT_BIT            #x00008000)
  (GL_EVAL_BIT            #x00010000)
  (GL_LIST_BIT            #x00020000)
  (GL_TEXTURE_BIT         #x00040000)
  (GL_SCISSOR_BIT         #x00080000)
  (GL_ALL_ATTRIB_BITS     #x000FFFFF))

(chdir "opengl")
(system "make --quiet libgl_init.so")
(load-extension "./libgl_init" "scm_init_gl")
(chdir "..")

(define libGL   (dynamic-link "libGL"))

(from-lib
 libGL
 (glClearColor void "glClearColor" float float float float)
 (glClear      void "glClear"      unsigned-int))

(define (clear-color red green blue alpha)
  "set the color for clear"
  (glClearColor red green blue alpha))

(define (clear mask)
  "clear elements of the current gl context"
  (glClear mask))
