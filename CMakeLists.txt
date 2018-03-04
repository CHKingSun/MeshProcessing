cmake_minimum_required(VERSION 3.9)
project(MeshProcessing)

set(CMAKE_CXX_STANDARD 11)

set(OPENGL_FILE D:/GL_VS)
include_directories(${OPENGL_FILE}/include)
link_directories(${OPENGL_FILE}/lib/x64)

set(ASSIMP_FILE D:/Assimp)
include_directories(${ASSIMP_FILE}/include)
link_directories(${ASSIMP_FILE}/lib/Debug)

include_directories(D:/stb)
include_directories(D:/glm/glm)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(${PROJECT_NAME} main.cpp Model.h Mesh.h Shader.h)
target_link_libraries(${PROJECT_NAME} assimp freeglut glew32)