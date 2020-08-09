#pragma once

#include <glad/glad.h>

#include <string>

namespace renderer {

GLuint load_shaders(const std::string& shader_name);

}
