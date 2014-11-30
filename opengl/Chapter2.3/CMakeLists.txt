add_executable(application4 main.cpp BulletOpenGLApplication.cpp)
target_link_libraries(application4 ${OPENGL_LIBRARIES} glut GL GLU ${BULLET_LIBRARIES})