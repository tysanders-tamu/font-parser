cmake_minimum_required(VERSION 3.0)

project(MyProject)

# Set C++ version
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add fmt library
add_subdirectory(fmt)


include_directories(
        ${PROJECT_SOURCE_DIR}/src
)

file(GLOB SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
)

# Add executable target
add_executable(out ${SOURCE_FILES})

# Link fmt library
target_link_libraries(out PRIVATE fmt::fmt glfw)
