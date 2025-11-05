#pragma once

#include <C:/OpenGLwrappers/glew-1.10.0-win32/glew-1.10.0/include/GL/glew.h>
#include <C:/OpenGLwrappers/freeglut-MSVC-2.8.1-1.mp/freeglut/include/GL/freeglut.h> 

void my_3d_projection(int width, int height)
{
  //GLdouble width_bound, height_bound;
  //width_bound = (GLdouble)width; height_bound = (GLdouble)height;
  GLdouble aspect = (GLdouble)width / (GLdouble)height;
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, aspect, 1.0, 50000.0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  //glOrtho(0.0, width_bound, 0.0, height_bound, 0.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

#define STRT_X_POS 25
#define STRT_Y_POS 25

void my_setup(int width, int height, char *window_name_str)
{
  glutInitContextVersion(4, 3);
  glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
  // For double buffering, uncomment the next line:
  // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  // Below line deos single buffering
  // If above line is uncommented, the next line will need to be commented out.
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(STRT_X_POS, STRT_Y_POS);
  glutCreateWindow(window_name_str);
  glewExperimental = GL_TRUE;
  glewInit();
  glutReshapeFunc(my_3d_projection);
}
