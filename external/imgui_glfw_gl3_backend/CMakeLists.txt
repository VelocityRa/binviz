add_library(imgui_glfw_gl3_backend STATIC imgui_impl_opengl3.cpp
                                          imgui_impl_opengl3.h
                                          imgui_impl_glfw.cpp
                                          imgui_impl_glfw.h)

target_compile_definitions(imgui_glfw_gl3_backend PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

target_link_libraries(imgui_glfw_gl3_backend PUBLIC glad::glad glfw)
