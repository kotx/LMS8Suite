cmake_minimum_required(VERSION 2.8)
project("lms8cli")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)

add_executable(lms8cli lms8cli.cpp)

target_link_libraries(lms8cli LMS8001)

set_target_properties(lms8cli
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(UNIX)
    target_link_libraries(lms8cli usb-1.0 pthread)
endif()
