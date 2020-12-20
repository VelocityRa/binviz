#include <gui.hpp>
#include <renderer.hpp>
#include <util/load_file.hpp>
#include <util/log.hpp>
#include <util/types.hpp>
#include <util/util.hpp>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <nfd.h>

#include <tuple>

// Function prototypes
void GLAPIENTRY gl_debug_message_callback(GLenum source,
                                          GLenum type,
                                          GLuint id,
                                          GLenum severity,
                                          GLsizei length,
                                          const GLchar* message,
                                          const void* userParam);
void glfw_error_callback(s32 error, const char* description);
void glfw_key_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mode);
void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void glfw_cursor_position_callback(GLFWwindow* window, f64 xpos, f64 ypos);
void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void glfw_window_size_callback(GLFWwindow* window, int width, int height);

namespace config {

constexpr bool is_release = true;

constexpr bool gl_debug = false; //!is_release;

constexpr char* window_title = "BinViz v0.1";

constexpr bool window_fullscreen = is_release ? false : false;  // border-less
constexpr bool vsync = true;

#if _4K
constexpr u32 window_width = 3840;
constexpr u32 window_height = window_fullscreen ? 2160 : 2160 - 130;
#else
constexpr u32 window_width = 1920;
constexpr u32 window_height = 1080;  //- 100;
#endif

}  // namespace config

Gui g_gui;
Renderer g_renderer;

s32 main(int argc, char** argv) {
    logging::init();

    // Init GLFW
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();

    // Set all the required options for GLFW
    if (config::gl_debug) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    } else {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    }
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    // glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window{};

    if (config::window_fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        window = glfwCreateWindow(mode->width, mode->height, config::window_title, monitor, nullptr);
    } else {
        window = glfwCreateWindow(config::window_width, config::window_height, config::window_title, nullptr, nullptr);
    }
    glfwMakeContextCurrent(window);
    if (window == nullptr) {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    // Enable vsync
    glfwSwapInterval(config::vsync ? 1 : 0);

    // Set the required callback functions
    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(window, glfw_cursor_position_callback);
    glfwSetScrollCallback(window, glfw_scroll_callback);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("Failed to initialize OpenGL context");
        glfwTerminate();
        return -1;
    }

    if constexpr (config::gl_debug) {
        // During init, enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        // TODO: glDebugMessageControl
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_message_callback, 0);
    }

    g_renderer.set_viewport_size({ config::window_width, config::window_height });
    g_renderer.init();

    char filename[256]{};

    if (argc >= 2)
        strncpy(filename, argv[1], sizeof(filename));
    else {
        nfdchar_t* filename_tmp = NULL;
        nfdresult_t result = NFD_OpenDialog(NULL, NULL, &filename_tmp);

        if (result == NFD_OKAY) {
            strncpy(filename, filename_tmp, sizeof(filename));
            free(filename_tmp);
        } else if (result == NFD_CANCEL) {
            spdlog::error("User pressed cancel.");
            return -1;
        } else {
            spdlog::error("Error: {}", NFD_GetError());
            return -2;
        }
    }

    const auto new_window_title = fmt::format("{}   [{}]", config::window_title, fs::path(filename).filename().string());
    glfwSetWindowTitle(window, new_window_title.c_str());

    auto file_data = util::load_file(filename);
    const auto file_size = file_data.size();

    g_renderer.set_data(std::move(file_data));
#if 0
  g_renderer.set_texture_size({ config::window_width, config::window_height });
#else
    int width = 2;
    for (; width < config::window_width; width *= 2);
    width /= 2;

    width = std::min(size_t(width), file_size);
    int height = std::min(size_t(600), file_size / width);

    g_renderer.set_texture_size({ width, height });
    // todo: std min
    //g_renderer.set_texture_size({ width, config::window_height });
#endif

    // Set up gui
    g_gui.init(&g_renderer, window, "#version 330");

    // Define the viewport dimensions
    glViewport(0, 0, config::window_width, config::window_height);

    // g_renderer.set_window_size(config::window_width, config::window_height);
    // g_renderer.init();

    // g_gui.set_g_renderer(&g_renderer);

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding
        // response functions
        glfwPollEvents();

        // Render

        g_gui.frame_begin();
        g_gui.draw_ui();

        // g_renderer.update();
        g_renderer.render();

        g_gui.frame_end();

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    g_gui.deinit();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// TODO: GUI owns everything currently, maybe a bad idea

void glfw_key_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mode) {
    bool pressed = (action == GLFW_PRESS);
    bool held = (action != GLFW_RELEASE);  // We want to catch repeats too
    bool released = (action == GLFW_RELEASE);

    if (pressed) {
        switch (key) {  // TODO: could use glfwGetWindowUserPointer
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_PAGE_UP:
                g_renderer.change_offset_page(-1);
                break;
            case GLFW_KEY_PAGE_DOWN:
                g_renderer.change_offset_page(1);
                break;
            default:;  // LOG_DEBUG("Pressed key: {}", key);
        }
    }
}

void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    const bool left_btn = (button == GLFW_MOUSE_BUTTON_LEFT);
    const bool pressed = (action == GLFW_PRESS);

    if (left_btn) {
        g_gui.left_mouse_pressed = pressed;

        double x{};
        double y{};
        glfwGetCursorPos(window, &x, &y);
        g_gui.prev_pos.x = x;
        g_gui.prev_pos.y = y;
    }
}

void glfw_cursor_position_callback(GLFWwindow* window, f64 mouse_x, f64 mouse_y) {
    if (g_gui.left_mouse_pressed && !g_gui.want_capture_read()) {
        glm::vec2 mouse_pos(mouse_x, mouse_y);
        glm::vec2 delta_pos = mouse_pos - g_gui.prev_pos;

        g_gui.renderer->change_pos(delta_pos);
        g_gui.prev_pos = mouse_pos;
    }
}

void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (g_gui.want_capture_read())
        return;

    float scale_delta = 1.09;

    if (yoffset > 0.0)
        scale_delta = 1.0 / scale_delta;

    g_gui.renderer->change_scale(scale_delta);
}

void glfw_error_callback(s32 error, const char* description) {
    LOG_ERROR("GLFW Error {}: {}", error, description);
    assert(0);
}

void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
    g_renderer.set_viewport_size({ width, height });
}

void GLAPIENTRY gl_debug_message_callback(GLenum source,
                                          GLenum type,
                                          GLuint id,
                                          GLenum severity,
                                          GLsizei length,
                                          const GLchar* message,
                                          const void* userParam) {
    const bool is_notification = (severity == GL_DEBUG_SEVERITY_NOTIFICATION);

    if (is_notification)
        return;

    const bool is_error = (type == GL_DEBUG_TYPE_ERROR);

    if (is_error) {
        LOG_ERROR("GL callback (error): type = 0x{:x}, message = {}", type, message);
        // assert(0);
    } else
        LOG_WARN("GL callback: type = 0x{:x}, severity = 0x{:x} message = {}", type, severity, message);
}
