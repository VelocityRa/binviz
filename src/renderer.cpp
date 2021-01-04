#include <renderer.hpp>

#include <shader.hpp>
#include <util/hsv.hpp>
#include <util/util.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>
#include <utility>

const GLuint ATTRIB_INDEX_POSITION = 0;
const GLuint ATTRIB_INDEX_TEXCOORD = 1;

#define RGBA_MODE 0

Renderer::Renderer(glm::uvec2 viewport_size) : m_viewport_size(viewport_size) {
    init();
}

void Renderer::init() {
    // Load and compile shaders
    m_shader_program_screen = renderer::load_shaders("screen");

    if (!m_shader_program_screen)
        throw std::runtime_error("Couldn't compile screen shader");

    palette_rainbow();

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

    is_inited = true;
}

void Renderer::calc_and_upload_screen_quad() {
    const float left_px = m_screen_pos.x;
    const float right_px = left_px + m_texture_size.x;
    const float bottom_px = m_texture_size.y - m_screen_pos.y;
    const float top_px = bottom_px - m_texture_size.y;

    float ratio_x = float(m_texture_size.x) / m_viewport_size.x;
    float ratio_y = float(m_texture_size.y) / m_viewport_size.y;

    // [0, 1] to [-1, 1]
    auto to_clip_space = [](float coord) -> float { return coord * 2.0 - 1.0; };

    float left_cs = to_clip_space(left_px / m_texture_size.x * ratio_x) / m_scale;
    float right_cs = to_clip_space(right_px / m_texture_size.x * ratio_x) / m_scale;
    float top_cs = to_clip_space(top_px / m_texture_size.y * ratio_y) / m_scale;
    float bottom_cs = to_clip_space(bottom_px / m_texture_size.y * ratio_y) / m_scale;

    const float vertices[] = {
        // Position Texcoords
        left_cs,  top_cs,    0.0f, 1.0f,  // Top-left
        left_cs,  bottom_cs, 0.0f, 0.0f,  // Bottom-left
        right_cs, top_cs,    1.0f, 1.0f,  // Top-right
        right_cs, bottom_cs, 1.0f, 0.0f,  // Bottom-right
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    is_screen_quad_updated = true;
}

void Renderer::upload_screen_texture() {
    bind_screen_texture();

    glPixelStorei(GL_UNPACK_ROW_LENGTH, m_texture_size.x);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_size.x, m_texture_size.y, GL_RGBA, GL_UNSIGNED_BYTE,
                    m_texture_data.data());
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    is_texture_uploaded = true;
}

void Renderer::render() {
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
    glUniform2f(m_u_tex_size, (float)m_texture_size.x, (float)m_texture_size.y);

    // Draw screen
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Renderer::set_texture_size(glm::ivec2 texture_size) {
    texture_size = glm::max(texture_size, { 1, 1 });

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

void Renderer::set_offset(s64 offset) {
    m_texture_data_offset = std::clamp(s64(offset), 0ll, s64(m_data.size()));

    is_texture_updated = false;
}

void Renderer::set_offset_page(s64 page) {
    const auto cur_size = m_texture_size;

    s32 offset = cur_size.x * cur_size.y * page;
    if (four_byte_stride)
        offset *= 4;

    set_offset(offset);
}

void Renderer::change_offset_page(s32 page_delta) {
    const auto cur_size = m_texture_size;
    const s32 offset = m_texture_data_offset;

    s32 delta = cur_size.x * cur_size.y * page_delta;
    if (four_byte_stride)
        delta *= 4;

    set_offset(offset + delta);
}

void Renderer::set_data(buffer&& data) {
    m_data = std::move(data);

    is_texture_updated = false;
}

void Renderer::set_viewport_size(glm::uvec2 viewport_size) {
    m_viewport_size = viewport_size;

    is_screen_quad_updated = false;
}

void Renderer::change_scale(float scale_mult) {
    m_scale *= scale_mult;

    is_screen_quad_updated = false;
}

void Renderer::update_texture() {
    auto data_size = m_data.size();
    auto tex_data_offset = m_texture_data_offset;

    m_texture_data.clear();
    m_texture_data.resize(data_size);

    const auto max_data_size = data_size - tex_data_offset;

#if RGBA_MODE
    for (s32 i = 0; i < max_data_size; i += 4) {
        const u32 val = *((u32*)&m_data[m_texture_data_offset + i]);
        const u8 r = (val >> 0) & 0xFF;
        const u8 g = (val >> 8) & 0xFF;
        const u8 b = (val >> 16) & 0xFF;

        m_texture_data[i / 4] = COLOR_RGBI_TO_U32(r, g, b);
    }

    is_texture_updated = true;
    is_texture_uploaded = false;
    return;
#endif

    if (shade_bytes_grayscale) {
        if (four_byte_stride) {
            for (s32 i = 0; i < max_data_size; i += 4) {
                const u8 val = m_data[m_texture_data_offset + i];
                const u8 r = val;
                const u8 g = val;
                const u8 b = val;

                m_texture_data[i / 4] = COLOR_RGBI_TO_U32(r, g, b);
            }
        } else {
            for (s32 i = 0; i < max_data_size; ++i) {
                const u8 val = m_data[m_texture_data_offset + i];
                const u8 r = val;
                const u8 g = val;
                const u8 b = val;

                m_texture_data[i] = COLOR_RGBI_TO_U32(r, g, b);
            }
        }
    }

    switch (draw_mode) {
        case DrawMode::Thresholding: {
            auto threshold_body = [&](s32& i, f32 val) {
                if (val == 0.0)
                    return;

                u32 color{};

                if (-0.0001 <= val && val <= 0.0001)
                    return;

                if (four_byte_stride) {
                    for (const auto& range : float_ranges) {
                        if (range.enabled && range.start <= val && val <= range.end) {
                            m_texture_data[i / 4] = range.color;

                            break;
                        }
                    }
                } else {
                    for (const auto& range : float_ranges) {
                        if (range.enabled && range.start <= val && val <= range.end) {
                            m_texture_data[i + 0] = range.color;
                            m_texture_data[i + 1] = range.color;
                            m_texture_data[i + 2] = range.color;
                            m_texture_data[i + 3] = range.color;

                            if (m_unaligned_floats)
                                i += 3;  // and loop statement will increase i by 1 more
                            break;
                        }
                    }
                }
            };

            if (m_unaligned_floats) {
                for (s32 i = 0; i < max_data_size; ++i) {
                    const size_t offset = m_texture_data_offset + i;
                    const f32 val = *((f32*)&m_data[offset]);

                    threshold_body(i, val);
                }
            } else {
                for (s32 i = 0; i < max_data_size; i += 4) {
                    const size_t offset = ((m_texture_data_offset + i) / 4) * 4;  // Align to 4
                    const f32 val = *((f32*)&m_data[offset]);

                    threshold_body(i, val);
                }
            }
            break;
        }
        case DrawMode::Paletted: {
            for (s32 i = 0; i < max_data_size; i++) {
                const u8 val = m_data[size_t(m_texture_data_offset + i)];
                m_texture_data[i] = palette_colors[val];
            }

            break;
        }
    }

    is_texture_updated = true;
    is_texture_uploaded = false;
}

void Renderer::palette_shuffle() {
    auto rng = std::default_random_engine{};

    auto it_shuffle_start = palette_colors.begin();

    if (palette_0_is_black)
        ++it_shuffle_start;

    std::shuffle(it_shuffle_start, palette_colors.end(), rng);
}

void Renderer::palette_rainbow() {
    auto palette = stx::create_array<u8, 256>();

    for (int i = 0; i <= 0xFF; ++i) {
        HSV hsv;
        hsv.h = f32(palette[i]) / 255.0 * 360.0;
        hsv.s = 1.0;
        hsv.v = 0.4 + ((f32(i & 0xF) / 0xF) * 0.6);

        RGB rgb = hsv2rgb(hsv);

        palette_colors[i] = COLOR_RGBI_TO_U32(u8(rgb.r * 255), u8(rgb.g * 255), u8(rgb.b * 255));
    }

    if (palette_0_is_black)
        palette_colors[0] = COLOR_RGBI_TO_U32(0, 0, 0);
}

void Renderer::palette_golden_angle() {
    auto palette = stx::create_array<u8, 256>();

    for (auto i = 0; i <= 0xFF; ++i) {
        HSV hsv;
        hsv.h = fmod(137.508 * i, 360.0);
        hsv.s = 0.6 + (hsv.h / 360.0 * 0.4);
        hsv.v = 0.4 + (hsv.h / 360.0 * 0.5);

        RGB rgb = hsv2rgb(hsv);

        palette_colors[i] = COLOR_RGBI_TO_U32(u8(rgb.r * 255), u8(rgb.g * 255), u8(rgb.b * 255));
    }

    if (palette_0_is_black)
        palette_colors[0] = COLOR_RGBI_TO_U32(0, 0, 0);
}

void Renderer::set_pos(glm::vec2 pos) {
    m_screen_pos = pos;

    is_screen_quad_updated = false;
}

void Renderer::change_pos(glm::vec2 pos_delta) {
    m_screen_pos += pos_delta * m_scale;

    is_screen_quad_updated = false;
}

void Renderer::bind_screen_texture() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex_screen);
}

Renderer::~Renderer() {
    if (is_inited) {
        glDeleteTextures(1, &m_tex_screen);
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
        glDeleteProgram(m_shader_program_screen);
    }
}
