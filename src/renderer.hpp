#pragma once

#include <util/types.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>

#define COLOR_RGBI_TO_U32(r, g, b) u32(0xFF << 24) | (b << 16) | (g << 8) | (r << 0)
#define COLOR_U32_TO_RGBF(c) \
    { f32((c >> 0) & 0xFF) / 255, f32((c >> 8) & 0xFF) / 255, f32((c >> 16) & 0xFF) / 255 }

class Renderer {
   public:
    explicit Renderer(glm::uvec2 viewport_size);
    ~Renderer();

    void render();
    void bind_screen_texture() const;
    void set_data(buffer&& data);
    void set_viewport_size(glm::uvec2 viewport_size);
    void set_texture_size(glm::ivec2 texture_size);
    void set_offset(s64 offset);

    void set_pos(glm::vec2 pos);
    void change_pos(glm::vec2 pos_delta);
    void change_scale(f32 scale_mult);

    void calc_and_upload_screen_quad();
    void upload_screen_texture();
    void update_texture();

    void palette_shuffle();
    void palette_rainbow();
    void palette_golden_angle();

    glm::ivec2 m_texture_size{};
    glm::uvec2 m_viewport_size{};

    glm::vec2 m_screen_pos{};

    f32 m_scale{ 1.0 };

    s32 m_texture_data_offset{};
    std::vector<u32> m_texture_data;

    buffer m_data;

    bool is_texture_updated{};
    bool is_texture_uploaded{};
    bool is_screen_quad_updated{};

    struct ColoredFloatRange {
        f32 start;
        f32 end;
        u32 color;
        bool enabled;
    };
    std::array<u32, 256> palette_colors;

    enum class DrawMode { Thresholding, Paletted };

    bool shade_bytes_grayscale{ true };
    bool four_byte_stride{ false };
    DrawMode draw_mode{ DrawMode::Thresholding };

    std::array<ColoredFloatRange, 3> float_ranges = {
        ColoredFloatRange{ -1.0, 1.0, COLOR_RGBI_TO_U32(0x00, 0xFF, 0x00), true },
        ColoredFloatRange{ 0, 10000.0, COLOR_RGBI_TO_U32(0xFF, 0x00, 0x00), true },
        ColoredFloatRange{ -10000.0, 0, COLOR_RGBI_TO_U32(0x00, 0x00, 0xFF), true },
    };

    // Shaders
    GLuint m_shader_program_screen{};

    // Other OpenGL objects
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_tex_screen{};
    GLuint m_u_tex_size{};
};
