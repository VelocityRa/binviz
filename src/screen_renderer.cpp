#include <screen_renderer.hpp>

#include <shader.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <stdexcept>

const GLuint ATTRIB_INDEX_POSITION = 0;
const GLuint ATTRIB_INDEX_TEXCOORD = 1;

ScreenRenderer::ScreenRenderer(s32 width, s32 height) : m_screen_width(width), m_screen_height(height) {
  // Load and compile shaders
  m_shader_program_screen = renderer::load_shaders("screen");

  if (!m_shader_program_screen)
    throw std::runtime_error("Couldn't compile screen shader");

  glUseProgram(m_shader_program_screen);

  // Generate VAO
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  // Generate and configure VBO
  glGenBuffers(1, &m_vbo);

  calc_and_upload_screen_quad();

  const auto vertex_stride = 4 * sizeof(float);
  const auto position_offset = 0 * sizeof(float);
  const auto texcoord_offset = 2 * sizeof(float);

  glEnableVertexAttribArray(ATTRIB_INDEX_POSITION);
  glVertexAttribPointer(ATTRIB_INDEX_POSITION, 2, GL_FLOAT, GL_FALSE, vertex_stride, (const void*)position_offset);

  glEnableVertexAttribArray(ATTRIB_INDEX_TEXCOORD);
  glVertexAttribPointer(ATTRIB_INDEX_TEXCOORD, 2, GL_FLOAT, GL_FALSE, vertex_stride, (const void*)texcoord_offset);

  // Generate and configure screen texture
  glGenTextures(1, &m_tex_screen);
  bind_screen_texture();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Get uniforms
  m_u_tex_size = glGetUniformLocation(m_shader_program_screen, "u_tex_size");

  glBindVertexArray(0);
}

void ScreenRenderer::calc_and_upload_screen_quad() {
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

  s32 left_px = m_screen_pos.x;
  s32 right_px = left_px + m_screen_width;
  s32 bottom_px = m_screen_height - m_screen_pos.y;
  s32 top_px = bottom_px - m_screen_height;

  // [0, 1] to [-1, 1]
  auto to_clip_space = [](float coord) -> float { return coord * 2.0 - 1.0; };

  // float left = to_clip_space(left_px / 512);
  // float right = to_clip_space(right_px / 512);
  // float top = to_clip_space(top_px / 512);
  // float bottom = to_clip_space(bottom_px / 512);

  // todo
  glm::uvec2 scale(m_screen_width * m_scale, m_screen_height * m_scale);

  float left = to_clip_space(f32(left_px) / scale.x);
  float right = to_clip_space(f32(right_px) / scale.x);
  float top = to_clip_space(f32(top_px) / scale.y);
  float bottom = to_clip_space(f32(bottom_px) / scale.y);

  const float vertices[] = {
    // Position Texcoords
    left,  top,    0.0f, 0.0f,  // Top-left
    left,  bottom, 0.0f, 1.0f,  // Bottom-left
    right, top,    1.0f, 0.0f,  // Top-right
    right, bottom, 1.0f, 1.0f,  // Bottom-right
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void ScreenRenderer::render(const void* vram_data) const {
  glClearColor(0.1, 0.1, 0.1, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // Bind needed state
  glBindVertexArray(m_vao);
  glUseProgram(m_shader_program_screen);
  bind_screen_texture();

  // Upload screen texture
  glPixelStorei(GL_UNPACK_ROW_LENGTH, m_screen_width);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_screen_width, m_screen_height, GL_RGBA, GL_UNSIGNED_BYTE, vram_data);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

  // Set uniforms
  glUniform2f(m_u_tex_size, (f32)m_screen_width, (f32)m_screen_height);

  // Draw screen
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ScreenRenderer::set_texture_size(s32 width, s32 height) {
  bind_screen_texture();

  // If screen texture dimensions changed
  if (width != m_screen_width || height != m_screen_height) {
    // Configure texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    m_screen_width = width;
    m_screen_height = height;
  }
}

void ScreenRenderer::change_scale(float scale_mult) {
  m_scale *= scale_mult;

  calc_and_upload_screen_quad();
}

void ScreenRenderer::change_pos(glm::vec2 pos_delta) {
  m_screen_pos += pos_delta * m_scale;

  calc_and_upload_screen_quad();
}

void ScreenRenderer::bind_screen_texture() const {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_tex_screen);
}

ScreenRenderer::~ScreenRenderer() {
  glDeleteTextures(1, &m_tex_screen);
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
  glDeleteProgram(m_shader_program_screen);
}
