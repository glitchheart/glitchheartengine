#include once "GL/gl.bi"
#include once "GL/glu.bi"
#include once "soil.bi"

' test of RGBA texture (with alpha channel)
ChDir ExePath()

ScreenRes 640,480,32,,2

var texture = SOIL_load_OGL_texture("OpenGL_RGBA.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO)

dim as integer w,h
screeninfo w,h
glViewport 0,0,w,h
glMatrixMode GL_PROJECTION
glLoadIdentity
gluPerspective 45.0, w/h, 0.1, 100.0
glMatrixMode GL_MODELVIEW
glLoadIdentity

glClearColor 1,0,0,1
glShadeModel GL_SMOOTH

glDisable GL_DEPTH_TEST
glHint GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST

glEnable GL_BLEND
glBlendFunc GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA

glEnable GL_TEXTURE_2D
glBindTexture GL_TEXTURE_2D, texture
do
  glClear GL_COLOR_BUFFER_BIT
  glBegin GL_QUADS
    glTexCoord2f 0,0:glVertex3f -.5, .25,-5
    glTexCoord2f 1,0:glVertex3f  .5, .25,-5
    glTexCoord2f 1,1:glVertex3f  .5,-.25,-5
    glTexCoord2f 0,1:glVertex3f -.5,-.25,-5
  glEnd
  flip
loop while inkey=""


