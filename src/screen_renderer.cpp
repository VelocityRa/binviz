#include <screen_renderer.hpp>

#include <shader.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <stdexcept>
#include <utility>

const GLuint ATTRIB_INDEX_POSITION = 0;
const GLuint ATTRIB_INDEX_TEXCOORD = 1;

ScreenRenderer::ScreenRenderer(glm::uvec2 viewport_size) : m_viewport_size(viewport_size) {
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

  is_screen_quad_updated = true;
}

void ScreenRenderer::calc_and_upload_screen_quad() {
  s32 left_px = m_screen_pos.x;
  s32 right_px = left_px + m_texture_size.x;
  s32 bottom_px = m_texture_size.y - m_screen_pos.y;
  s32 top_px = bottom_px - m_texture_size.y;

  // [0, 1] to [-1, 1]
  auto to_clip_space = [](float coord) -> float { return coord * 2.0 - 1.0; };

  glm::uvec2 scale(m_viewport_size.x * m_scale, m_viewport_size.y * m_scale);

  float left_cs = to_clip_space(f32(left_px) / scale.x);
  float right_cs = to_clip_space(f32(right_px) / scale.x);
  float top_cs = to_clip_space(f32(top_px) / scale.y);
  float bottom_cs = to_clip_space(f32(bottom_px) / scale.y);

  const float vertices[] = {
    // Position Texcoords
    left_cs,  top_cs,    0.0f, 1.0f,  // Top-left
    left_cs,  bottom_cs, 0.0f, 0.0f,  // Bottom-left
    right_cs, top_cs,    1.0f, 1.0f,  // Top-right
    right_cs, bottom_cs, 1.0f, 0.0f,  // Bottom-right
  };

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void ScreenRenderer::upload_screen_texture() {
  bind_screen_texture();

  glPixelStorei(GL_UNPACK_ROW_LENGTH, m_texture_size.x);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_size.x, m_texture_size.y, GL_RGBA, GL_UNSIGNED_BYTE,
                  m_texture_data.data());
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

  is_texture_uploaded = true;
}

void ScreenRenderer::render() {
  glClearColor(0.1, 0.1, 0.1, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // Bind needed state
  glBindVertexArray(m_vao);
  glUseProgram(m_shader_program_screen);
  bind_screen_texture();

  if (!is_screen_quad_updated)
    calc_and_upload_screen_quad();

  // Update texture
  if (!is_texture_updated)
    update_texture();

  // Upload texture
  if (!is_texture_uploaded)
    upload_screen_texture();

  // Set uniforms
  glUniform2f(m_u_tex_size, (f32)m_texture_size.x, (f32)m_texture_size.y);

  // Draw screen
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ScreenRenderer::set_texture_size(glm::ivec2 texture_size) {
  // texture_size.y = m_data.size() / texture_size.x;
  // texture_size.y = 14400;

  // If screen texture dimensions changed
  if (texture_size != m_texture_size) {
    bind_screen_texture();

    // Configure texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_size.x, texture_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    m_texture_size = texture_size;

    is_screen_quad_updated = false;
    is_texture_updated = false;
  }
}

void ScreenRenderer::set_offset(u64 offset) {
  m_texture_data_offset = offset;

  is_texture_updated = false;
}

void ScreenRenderer::set_data(buffer&& data) {
  m_data = std::move(data);

  is_texture_updated = false;
}

void ScreenRenderer::set_viewport_size(glm::uvec2 viewport_size) {
  m_viewport_size = viewport_size;

  is_screen_quad_updated = false;
}

void ScreenRenderer::change_scale(float scale_mult) {
  m_scale *= scale_mult;

  is_screen_quad_updated = false;
}

void ScreenRenderer::update_texture() {
  const auto count = m_texture_size.x * m_texture_size.y;
  m_texture_data.resize(count);

#if 0
  for (u32 y = 0; y < height; ++y) {
    for (u32 x = 0; x < width; ++x) {
      const u8 r = f32(y) / height * 255;
      const u8 g = f32(x) / width * 255;
      // const u8 r = (y % 32) * 8;
      // const u8 g = (x % 32) * 8;
      // const u8 b = 0x00;
      const u8 a = 0xFF;

      const auto idx = y * width + x;

      m_texture_data[idx] = (a << 24) | (b << 16) | (g << 8) | (r << 0);
    }
  }
#endif

  for (u32 i = 0; i < count; ++i) {
    const u8 val = m_data[m_texture_data_offset + i];
    const u8 r = val;
    const u8 g = val;
    const u8 b = val;
    const u8 a = 0xFF;

    m_texture_data[i] = (a << 24) | (b << 16) | (g << 8) | (r << 0);
  }

  is_texture_updated = true;
  is_texture_uploaded = false;
}

void ScreenRenderer::change_pos(glm::vec2 pos_delta) {
  m_screen_pos += pos_delta * m_scale;

  is_screen_quad_updated = false;
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
