#include <gui.hpp>

#include <screen_renderer.hpp>
#include <util/types.hpp>

#include <imgui.h>
#include <imgui_glfw_gl3_backend/imgui_impl_glfw.h>
#include <imgui_glfw_gl3_backend/imgui_impl_opengl3.h>

#include <algorithm>
#include <array>
#include <string>

namespace ImGui {

static auto vector_getter = [](void* vec, int idx, const char** out_text) {
  auto& vector = *static_cast<std::vector<std::string>*>(vec);
  if (idx < 0 || idx >= static_cast<int>(vector.size())) {
    return false;
  }
  *out_text = vector.at(idx).c_str();
  return true;
};

bool Combo(const char* label, int* currIndex, std::vector<std::string>& values) {
  if (values.empty()) {
    return false;
  }
  return Combo(label, currIndex, vector_getter, static_cast<void*>(&values), values.size());
}

bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values) {
  if (values.empty()) {
    return false;
  }
  return ListBox(label, currIndex, vector_getter, static_cast<void*>(&values), values.size());
}

}  // namespace ImGui

void Gui::init(ScreenRenderer* _renderer, GLFWwindow* window, const char* glsl_version) {
  renderer = _renderer;
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 3 });
  // ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 4, 4 });
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4, 4 });
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void Gui::frame_begin() {
  if (!active)
    return;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Gui::draw_ui() {
  if (!active)
    return;

  // ImGui::ShowDemoWindow();

  // Renderer window
  if (ImGui::Begin("Controls")) {
    const auto cur_size = renderer->m_texture_size;

    s32 new_x = cur_size.x;
    if (ImGui::SliderInt("Width", &new_x, 1, 2560)) {
      renderer->set_texture_size({ new_x, cur_size.y });
    }

    ImGui::SameLine();

    constexpr std::array width_values = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };
    const char* const width_values_str[] = { "8", "16", "32", "64", "128", "256", "512", "1024", "2048" };

    static s32 width_idx{};
    ImGui::SetNextItemWidth(5 * ImGui::GetFontSize());
    if (ImGui::Combo("##width", &width_idx, width_values_str, IM_ARRAYSIZE(width_values_str))) {
      renderer->set_texture_size({ width_values[width_idx], cur_size.y });
    }

    const auto data_size = renderer->m_data.size();

    s32 new_y = cur_size.y;
    const auto max_y = data_size / cur_size.x;
    if (ImGui::DragInt("Height", &new_y, 1.0, 1, max_y)) {
      renderer->set_texture_size({ cur_size.x, new_y });
    }

    s32 offset = renderer->m_texture_data_offset;
    if (ImGui::InputInt("Offset", &offset, 1, cur_size.x, ImGuiInputTextFlags_CharsHexadecimal)) {
      if (offset >= 0 && offset < data_size)
        renderer->set_offset(offset);
    }
    ImGui::SameLine();
    if (ImGui::Button("PgUp")) {
      offset = std::max(0, offset - cur_size.x * cur_size.y);

      renderer->set_offset(offset);
    }
    ImGui::SameLine();
    if (ImGui::Button("PgDown")) {
      offset = std::min(size_t(offset + cur_size.x * cur_size.y), data_size);

      renderer->set_offset(offset);
    }

    ImGui::Separator();

    if (ImGui::DragFloat("Scale", &renderer->m_scale, 0.01f, 0.0f, 5.0f)) {
      renderer->is_screen_quad_updated = false;
    }
  }
  ImGui::End();

  bool open{};
  // if (ImGui::Begin("Info", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
  //    ImGui::Value("Scale", renderer->m_scale);
  //}
  // ImGui::End();

  // Performance window
  // TODO
}

void Gui::frame_end() {
  if (!active)
    return;

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::deinit() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
