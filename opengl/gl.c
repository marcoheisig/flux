#include <GL/gl.h>
#if defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#endif

void
scm_init_gl() {
    // @Julian put your code here
#if defined(__APPLE__)
    GLint swap = 1;
    CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &swap);
#endif
}
