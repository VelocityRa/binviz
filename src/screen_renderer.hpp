#pragma once

#include <util/types.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

class ScreenRenderer {
 public:
  explicit ScreenRenderer(glm::uvec2 viewport_size);
  ~ScreenRenderer();

  void render();
  void bind_screen_texture() const;
  void set_viewport_size(glm::uvec2 viewport_size);
  void set_texture_size(glm::ivec2 texture_size);
  void set_data(buffer&& data);

  void change_pos(glm::vec2 pos_delta);
  void change_scale(float scale_mult);

 public:
  glm::ivec2 m_texture_size{};

 private:
  void calc_and_upload_screen_quad();
  void upload_screen_texture();
  void update_texture();

 private:
  glm::uvec2 m_viewport_size{};

  glm::vec2 m_screen_pos{};

  float m_scale{ 1.0 };

 private:
  std::vector<u32> m_texture_data;

  buffer m_data;

  bool is_texture_updated{};
  bool is_texture_uploaded{};

  // Shaders
  GLuint m_shader_program_screen{};

  // Other OpenGL objects
  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_tex_screen{};
  GLuint m_u_tex_size{};
};
