add_executable(application2 main.cpp BulletOpenGLApplication.cpp)
target_link_libraries(application2 ${OPENGL_LIBRARIES} glut GL GLU ${BULLET_LIBRARIES})