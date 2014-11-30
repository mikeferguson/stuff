add_executable(application3 main.cpp BulletOpenGLApplication.cpp)
target_link_libraries(application3 ${OPENGL_LIBRARIES} glut GL GLU ${BULLET_LIBRARIES})