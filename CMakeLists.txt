
# Copyright (C) 2025-2026 Mykyta Polishyk
# This project is licensed under the GNU General Public License v3.0 or later.
# See the LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(OpenGD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

# Targets
file(GLOB SOURCES "src/*.cpp" "src/**/*.cpp")

add_library(glad STATIC lib/glad/src/glad.cpp)
target_include_directories(glad PUBLIC lib/glad/include)

add_executable(${PROJECT_NAME} ${SOURCES})

# Includes
target_include_directories(${PROJECT_NAME} PRIVATE
    lib/glm
    lib/simpleini
    src/includes
)
# Glad
target_link_libraries(OpenGD PRIVATE glad)

# GLFW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# OpenAL
find_package(OpenAL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenAL::OpenAL)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(OpenGD PRIVATE OpenGL::GL)

# PNG
find_package(PNG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE PNG::PNG)

# Assimp
find_package(assimp REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

# Freetype
find_package(Freetype REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Freetype::Freetype)

# SND file
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED sndfile)

# Includes
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${GLFW_CFLAGS_OTHER})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PRIVATE ${SNDFILE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SNDFILE_LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${SNDFILE_CFLAGS_OTHER})

