#pragma once

#include <util/types.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

class ScreenRenderer {
 public:
  explicit ScreenRenderer(s32 width, s32 height);
  ~ScreenRenderer();

  void render(const void* vram_data) const;
  void bind_screen_texture() const;
  void set_texture_size(s32 width, s32 height);

  void change_pos(glm::vec2 pos_delta);
  void change_scale(float scale_delta);

 private:
  void calc_and_upload_screen_quad();

 private:
  s32 m_screen_width{};
  s32 m_screen_height{};

  glm::vec2 m_screen_pos{};

  float m_scale{ 0.5 };

  // Shaders
  GLuint m_shader_program_screen{};

  // Other OpenGL objects
  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_tex_screen{};
  GLuint m_u_tex_size{};
};
