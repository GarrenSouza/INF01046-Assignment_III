cmake_minimum_required(VERSION 2.8)
project( image_app )

set(CMAKE_VERBOSE_MAKEFILE ON)

find_package( OpenCV REQUIRED )

include_directories("include")

add_executable(image_app src/main.cpp src/Input.cpp)

target_link_libraries(image_app ${OpenCV_LIBS})
