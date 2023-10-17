# Using modern CMake
cmake_minimum_required(VERSION 3.26)

# Name the project
project(xxs VERSION 0.1
            DESCRIPTION "xxs game engine"
            LANGUAGES CXX)

# Clean output directory 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/executable)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Add SDL3 support
add_subdirectory(external/sdl EXCLUDE_FROM_ALL)

# Add SDL Image support
add_subdirectory(external/sdl-image)

# Add lua
add_subdirectory(external/lua EXCLUDE_FROM_ALL)

# Add executable
add_executable(xxs
    code/source/main.cpp
    code/source/fileio.cpp
    code/source/device.cpp
    code/source/render.cpp
    code/source/input.cpp
    code/source/game.cpp
    code/source/input.cpp
    code/source/script_lua.cpp)

# Add include directories
target_include_directories(xxs
    PUBLIC code/include
    PRIVATE external/
    PRIVATE external/json
    PRIVATE external/sdl
    PRIVATE external/glm
    PRIVATE external/lua
    PRIVATE external/sol2/include
    PRIVATE external/sdl-image/include)

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(xxs PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL3 library. SDL3::SDL3 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(xxs PRIVATE SDL3::SDL3)

# Link to the SDL Image library
target_link_libraries(xxs PRIVATE SDL3_image::SDL3_image)

# Link to the lua library
target_link_libraries(xxs PRIVATE lua)
