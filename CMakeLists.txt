cmake_minimum_required(VERSION 3.10)

project(RaylibExample)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directory
include_directories(include)

# Add the executable
add_executable(main src/main.cpp include/rlights.h)

# Link the raylib library and system libraries
if (WIN32)
    target_link_libraries(main ${CMAKE_SOURCE_DIR}/lib/raylib.lib opengl32 gdi32 winmm)
endif()

# Copy the raylib DLL to the build directory
add_custom_command(TARGET main POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   ${CMAKE_SOURCE_DIR}/dll/raylib.dll
                   $<TARGET_FILE_DIR:main>)
