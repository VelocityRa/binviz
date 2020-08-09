#include <gui.hpp>

#include <util/types.hpp>

#include <imgui.h>
#include <imgui_glfw_gl3_backend/imgui_impl_glfw.h>
#include <imgui_glfw_gl3_backend/imgui_impl_opengl3.h>

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
  // ImGui::StyleColorsDark();
  ImGui::StyleColorsClassic();

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 3 });
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 4, 4 });
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4, 4 });
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

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
