// Dear ImGui: standalone example application for GLFW + OpenGL 3, using
// programmable pipeline (GLFW is a cross-platform general purpose library for
// handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation,
// etc.) If you are new to Dear ImGui, read documentation from the docs/ folder
// + read the top of imgui.cpp. Read online:
// https://github.com/ocornut/imgui/tree/master/docs

#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "app_app.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace {
///
auto GetWindowFlags() {
  return ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
         ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
         ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
}

void OnFrame(vh::ponc::App& app) {
  auto& io = ImGui::GetIO();

  // if (m_Platform->HasWindowScaleChanged())
  //     m_Platform->AcknowledgeWindowScaleChanged();

  // if (m_Platform->HasFramebufferScaleChanged())
  // {
  //     RecreateFontAtlas();
  //     m_Platform->AcknowledgeFramebufferScaleChanged();
  // }

  // const float windowScale      = m_Platform->GetWindowScale();
  // const float framebufferScale = m_Platform->GetFramebufferScale();

  // if (io.WantSetMousePos)
  // {
  //     io.MousePos.x *= windowScale;
  //     io.MousePos.y *= windowScale;
  // }

  // m_Platform->NewFrame();

  // // Don't touch "uninitialized" mouse position
  // if (io.MousePos.x > -FLT_MAX && io.MousePos.y > -FLT_MAX)
  // {
  //     io.MousePos.x    /= windowScale;
  //     io.MousePos.y    /= windowScale;
  // }
  // io.DisplaySize.x /= windowScale;
  // io.DisplaySize.y /= windowScale;

  // io.DisplayFramebufferScale.x = framebufferScale;
  // io.DisplayFramebufferScale.y = framebufferScale;

  // m_Renderer->NewFrame();

  // ImGui::NewFrame();

  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(io.DisplaySize);
  // const auto windowBorderSize = ImGui::GetStyle().WindowBorderSize;
  // const auto windowRounding = ImGui::GetStyle().WindowRounding;
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::Begin("Content", nullptr, GetWindowFlags());
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, windowBorderSize);
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, windowRounding);

  app.OnFrame(io.DeltaTime);

  // ImGui::PopStyleVar(2);
  ImGui::End();
  // ImGui::PopStyleVar(2);

  // Rendering
  // m_Renderer->Clear(ImColor(32, 32, 32, 255));
  // ImGui::Render();
  // m_Renderer->RenderDrawData(ImGui::GetDrawData());

  // m_Platform->FinishFrame();
}
}  // namespace

///
auto main(int /*unused*/, char** /*unused*/) -> int {
  if (const auto glfw_failed = glfwInit() == 0) {
    return EXIT_FAILURE;
  }

  auto* window =
      glfwCreateWindow(1280, 720, "PROJECT_NAME - PONC", nullptr, nullptr);

  if (const auto window_failed = window == nullptr) {
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  ImGui::CreateContext();

  auto& io = ImGui::GetIO();
  // NOLINTNEXTLINE(*-signed-bitwise)
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  auto app = vh::ponc::App{nullptr, 0, nullptr};
  app.OnStart();

  while (const auto not_closed = glfwWindowShouldClose(window) == 0) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    OnFrame(app);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  app.OnStop();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}