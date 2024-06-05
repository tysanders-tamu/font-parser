cmake_minimum_required(VERSION 3.11)
project(font-parser)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_VERBOSE_MAKEFILE 0) # 1 should be used for debugging
set (CMAKE_SUPPRESS_REGENERATION TRUE) # Suppresses ZERO_CHECK

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++17")

#freetype?
find_package(Freetype REQUIRED)

#GLAD specific
set(GLAD_LIBRARIES dl)

#GFLW options
option (GLFW_INSTALL OFF)
option (GLFW_BUILD_DOCS OFF)
option (GLFW_BUILD_EXAMPLES OFF)
option (GLFW_BUILD_TESTS OFF)

#!UPDATE WHEN ADDING NEW DEPENDENCIES
# Add subdirectories for dependencies
add_subdirectory(deps/glfw)
add_subdirectory(deps/fmt)

#!UPDATE WHEN ADDING NEW DEPENDENCIES
# Include directories for dependencies
include_directories(
  src/
  deps/glad/include/
  deps/glfw/include/
  deps/fmt/include/
  ${FREETYPE_INCLUDE_DIRS}
)

# Collect all source, header, dependency files
file (GLOB DEPENDENCY_FILES deps/glad/src/glad.c)

file(GLOB_RECURSE HEADER_FILES
    src/*.h
    src/*.hpp
)

file(GLOB_RECURSE SOURCE_FILES
    src/*.cpp
)

file(GLOB CONFIG_FILES
    CMakeLists.txt
    .gitignore
    .gitmodules
)

#group
source_group("headers" FILES ${HEADER_FILES})
source_group("sources" FILES ${SOURCE_FILES})
source_group("dependencies" FILES ${DEPENDENCY_FILES})
source_group("config" FILES ${CONFIG_FILES})

# Set the output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Executable
add_executable(font-parser ${SOURCE_FILES} ${HEADER_FILES} ${DEPENDENCY_FILES} ${CONFIG_FILES})

# Link libraries
add_definitions (-DGLFW_INCLUDE_NONE
                 -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

target_link_libraries(${PROJECT_NAME}
                      fmt
                      glfw
                      ${GLFW_LIBRARIES}
                      ${GLAD_LIBRARIES}
                      ${FREETYPE_LIBRARIES}
                      )
                    