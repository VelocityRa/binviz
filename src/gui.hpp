#pragma once

#include <glm/glm.hpp>

class ScreenRenderer;
struct GLFWwindow;

class Gui {
 public:
  void init(ScreenRenderer* renderer, GLFWwindow* window, const char* glsl_version);

  void frame_begin();
  void draw_ui();
  void frame_end();
  void deinit();

 public:
  ScreenRenderer* renderer;
  bool active{ true };
  bool left_mouse_pressed{};
  glm::vec2 prev_pos{};
};
