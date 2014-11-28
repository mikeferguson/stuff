cmake_minimum_required(VERSION 2.8.3)
project(opengl_bullet_examples)

find_package(Bullet)
find_package(GLUT REQUIRED)
#if (OPENGL_FOUND)
#  set(gl_LIBS ${gl_LIBS} ${OPENGL_LIBRARIES})
#endif(OPENGL_FOUND)

include_directories(${GLUT_INCLUDE_DIR})

add_subdirectory(Chapter1.2)