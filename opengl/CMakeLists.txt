cmake_minimum_required(VERSION 2.8.3)
project(opengl_bullet_examples)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

find_package(Bullet)
find_package(GLUT REQUIRED)

include_directories(${GLUT_INCLUDE_DIR} ${BULLET_INCLUDE_DIRS})

add_subdirectory(Chapter1.2)
add_subdirectory(Chapter2.1)
add_subdirectory(Chapter2.2)
add_subdirectory(Chapter2.3)
