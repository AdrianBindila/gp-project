cmake_minimum_required(VERSION 3.21)
project(OpenGL_Project_Core)

set(CMAKE_CXX_STANDARD 14)

add_executable(OpenGL_Project_Core main.cpp Window.cpp Window.h SkyBox.cpp SkyBox.hpp Shader.hpp Shader.cpp Camera.hpp Camera.cpp Mesh.cpp Mesh.hpp Model3D.cpp Model3D.hpp stb_image.cpp stb_image.h tiny_obj_loader.cpp tiny_obj_loader.h)

target_link_libraries(OpenGL_Project_Core glfw GLEW GL)
