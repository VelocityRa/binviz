#pragma once

#include <glm/glm.hpp>

#include <vector>

class Renderer;
struct GLFWwindow;

#define _4K 1

class Gui {
 public:
  void init(Renderer* renderer, GLFWwindow* window, const char* glsl_version);

  void frame_begin();
  void draw_ui();
  void frame_end();
  void deinit();

  bool want_capture_read();

 public:
  Renderer* renderer;
  bool active{ true };
  bool left_mouse_pressed{};
  glm::vec2 prev_pos{};
};
