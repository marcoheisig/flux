#include <libguile.h>
#if defined(__APPLE__)
#include <GL/gl.h>
#include <OpenGL/OpenGL.h>
#include <SDL2/SDL.h>
#endif

SCM init_gl() {
#if defined(__APPLE__)
    GLint swap = 1;
    CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &swap);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#endif
    return SCM_BOOL_T;
}

void
scm_init_gl() {
    scm_c_define_gsubr("init-opengl", 0, 0, 0, init_gl);
}
