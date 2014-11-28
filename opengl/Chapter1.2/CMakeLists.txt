add_executable(application main.cpp BulletOpenGLApplication.cpp)
target_link_libraries(application ${gl_LIBS} ${OPENGL_LIBRARIES} glut)