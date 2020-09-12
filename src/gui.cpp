#include <gui.hpp>

#include <renderer.hpp>
#include <util/types.hpp>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_glfw_gl3_backend/imgui_impl_glfw.h>
#include <imgui_glfw_gl3_backend/imgui_impl_opengl3.h>
#include <spdlog/fmt/fmt.h>

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

void init_style() {
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    auto& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] =
        ImVec4(48.0 / 255, 78.0 / 255, 111.0 / 255, 140.0 / 255);  // ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    style.Alpha = 0.9f;

    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
}

void Gui::init(Renderer* _renderer, GLFWwindow* window, const char* glsl_version) {
    renderer = _renderer;
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;

    init_style();

    // ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 3 });
    // ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 4, 4 });
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4, 4 });
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    float monitor_x_scale, monitor_y_scale;
    glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &monitor_x_scale, &monitor_y_scale);

#if !_4K
    monitor_x_scale = 1.0;
#endif

    io.Fonts->AddFontFromFileTTF("../../../../data/fonts/Ruda-Bold.ttf", int(15 * monitor_x_scale));

    auto& style = ImGui::GetStyle();
    style.ScaleAllSizes(monitor_x_scale);
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

    ImGui::ShowDemoWindow();

    ImGuiIO& io = ImGui::GetIO();
    const auto font_size = ImGui::GetFontSize();

    // Renderer window
    bool open_dummy{};

    if (ImGui::Begin("Controls", &open_dummy, ImGuiWindowFlags_AlwaysAutoResize)) {
        const auto cur_size = renderer->m_texture_size;

        ImGui::PushItemWidth(font_size * 10);
        if (ImGui::CollapsingHeader("Data Layout", ImGuiTreeNodeFlags_DefaultOpen)) {
            s32 new_x = cur_size.x;
            if (ImGui::SliderInt("Width", &new_x, 1, 2560)) {
                renderer->set_texture_size({ new_x, cur_size.y });
            }

            ImGui::SameLine();

            constexpr std::array width_values = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };
            const char* const width_values_str[] = { "8", "16", "32", "64", "128", "256", "512", "1024", "2048" };

            static s32 width_idx{};
            ImGui::SetNextItemWidth(font_size * 5);
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
                renderer->set_offset(offset - cur_size.x * cur_size.y);
            }
            ImGui::SameLine();
            if (ImGui::Button("PgDown")) {
                renderer->set_offset(offset + cur_size.x * cur_size.y);
            }

            if (ImGui::CollapsingHeader("Display Mode", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::Checkbox("Shade bytes", &renderer->shade_bytes_grayscale))
                    renderer->update_texture();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Shade all bytes from black to white,\nif not otherwise shaded.");

                if (ImGui::BeginTabBar("Modes##modes")) {
                    if (ImGui::BeginTabItem("Threshold")) {
                        if (ImGui::Checkbox("4 bytes per pixel", &renderer->four_byte_stride)) {
                            auto new_size = renderer->m_texture_size;

                            if (renderer->four_byte_stride)
                                new_size.x /= 4;
                            else
                                new_size.x *= 4;

                            renderer->set_texture_size(new_size);
                        }

                        ImGui::NewLine();

                        u32 i{};
                        for (auto& range : renderer->float_ranges) {
                            if (ImGui::Checkbox(fmt::format("##range_{}", i).c_str(), &range.enabled))
                                renderer->update_texture();

                            ImGui::SameLine();
                            ImGui::Text("Range %u:", i);

                            ImGui::SameLine();
                            if (ImGui::InputFloat2(fmt::format("##threshold{}", i).c_str(), &range.start))
                                renderer->update_texture();
                            float new_color[3] = { f32((range.color << 0) & 0xFF) / 255.0,
                                                   f32((range.color >> 8) & 0xFF) / 255.0,
                                                   f32((range.color >> 16) & 0xFF) / 255.0 };

                            ImGui::SameLine();
                            ImGui::PushItemWidth(font_size * 5);
                            if (ImGui::ColorPicker3(fmt::format("##color{}", i).c_str(), new_color,
                                                    ImGuiColorEditFlags_NoInputs)) {
                                range.color = COLOR_RGBI_TO_U32(u8(new_color[0] * 255), u8(new_color[1] * 255),
                                                                u8(new_color[2] * 255));
                                renderer->update_texture();
                            }
                            ImGui::PopItemWidth();

                            i++;
                        }

                        renderer->draw_mode = Renderer::DrawMode::Thresholding;

                        ImGui::EndTabItem();
                    }
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Shade floating point number ranges.");

                    if (ImGui::BeginTabItem("Palette")) {
                        ImGui::PushItemWidth(int(font_size * 1.2));
                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 2, 2 });

                        for (int i = 0; i <= 0xFF; ++i) {
                            const auto c = renderer->palette_colors[i];
                            f32 colors[3] = COLOR_U32_TO_RGBF(c);

                            if (i % 16 == 0) {
                                ImGui::Text(fmt::format("0x{:02X}", i).c_str());
                                ImGui::SameLine(font_size * 3);
                            }

                            ImGui::ColorEdit3(fmt::format("##{}", i).c_str(), colors,
                                              ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel |
                                                  ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoBorder |
                                                  ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_InputRGB |
                                                  ImGuiColorEditFlags_NoPicker);
                            if ((i + 1) % 16 != 0)
                                ImGui::SameLine();
                        }

                        ImGui::PopStyleVar();
                        ImGui::PopItemWidth();
                        ImGui::NewLine();

                        if (ImGui::Button("Set: Rainbow")) {
                            renderer->palette_rainbow();
                            renderer->update_texture();
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Set: Golden Angle Cycle")) {
                            renderer->palette_golden_angle();
                            renderer->update_texture();
                        }

                        if (ImGui::Button("Shuffle")) {
                            renderer->palette_shuffle();
                            renderer->update_texture();
                        }

                        ImGui::SameLine();

                        ImGui::Checkbox("0x00 is black", &renderer->palette_0_is_black);

                        renderer->draw_mode = Renderer::DrawMode::Paletted;

                        ImGui::EndTabItem();
                    }
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Shade bytes according to a palette.");

                    ImGui::EndTabBar();
                }

                ImGui::Separator();
                if (ImGui::Button("Update Screen", { font_size * 8, f32(s32(font_size * 1.8)) })) {
                    renderer->update_texture();
                }
            }
        }

        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::DragFloat2("Position", (float*)&renderer->m_screen_pos, 0.01f, 0.0f, 5.0f, "%.2f")) {
                renderer->is_screen_quad_updated = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset##pos")) {
                renderer->set_pos({ 0, 0 });
            }

            if (ImGui::DragFloat("Scale", &renderer->m_scale, 0.001f, 0.0f, 5.0f)) {
                renderer->is_screen_quad_updated = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset##scale")) {
                renderer->m_scale = 1.0;
                renderer->is_screen_quad_updated = false;
            }
        }
        ImGui::PopItemWidth();
    }
    ImGui::End();

    // if (ImGui::Begin("View")) {
    //}
    // ImGui::End();

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

bool Gui::want_capture_read() {
    return ImGui::GetIO().WantCaptureMouse;
}
