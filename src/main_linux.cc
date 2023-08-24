/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "app_app.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

    app.OnFrame(io.DeltaTime);

    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT);
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