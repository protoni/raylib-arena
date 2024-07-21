cmake_minimum_required(VERSION 3.10)

project(RaylibExample)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directory
include_directories(include)

# List all header files
set(HEADER_FILES
    include/animation.h
    include/camera.h
    include/game.h
    include/player.h
    include/shader_handler.h
    include/terrain.h
    include/utils.h
    include/rlights.h
    include/logger.h
    include/debug.h
    include/settings.h
)

# List all source files
set(SOURCE_FILES
    src/animation.cpp
    src/camera.cpp
    src/game.cpp
    src/main.cpp
    src/player.cpp
    src/shader_handler.cpp
    src/terrain.cpp
    src/utils.cpp
    src/logger.cpp
    src/debug.cpp
)

# Add the executable
add_executable(main ${SOURCE_FILES} ${HEADER_FILES})

# Link the raylib library and system libraries
if (WIN32)
    target_link_libraries(main ${CMAKE_SOURCE_DIR}/lib/raylib.lib opengl32 gdi32 winmm)
endif()

# Copy the raylib DLL to the build directory
add_custom_command(TARGET main POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   ${CMAKE_SOURCE_DIR}/dll/raylib.dll
                   $<TARGET_FILE_DIR:main>)

# Set the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)

# Group the source files in the IDE
source_group("Header Files" FILES ${HEADER_FILES})
source_group("Source Files" FILES ${SOURCE_FILES})