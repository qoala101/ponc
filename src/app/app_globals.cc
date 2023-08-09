#include "app_globals.h"

#include <crude_json.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <iostream>
#include <memory>

#include "cpp_assert.h"

namespace vh::ponc {
namespace {
///
auto GetInitialized() -> auto& {
  static auto initialized = false;
  return initialized;
}

///
auto OnReadOpen(ImGuiContext* /*unused*/, ImGuiSettingsHandler* /*unused*/,
                const char* /*unused*/) -> void* {
  static auto kUnused = 0;
  return static_cast<void*>(&kUnused);
}
}  // namespace

///
auto GlobalsProxy::GetInstance() -> Globals& {
  Expects(GetInitialized());
  return Globals::GetInstance();
}

///
void GlobalsProxy::RegisterHandlers() {
  auto& initialized = GetInitialized();
  Expects(!initialized);

  Globals::RegisterHandlers();
  initialized = true;
}

///
auto Globals::GetInstance() -> Globals& {
  static auto globals = Globals{};
  return globals;
}

///
void Globals::RegisterHandlers() {
  auto handler = ImGuiSettingsHandler{};
  handler.TypeName = "Ponc";
  handler.TypeHash = ImHashStr(handler.TypeName);
  handler.ReadOpenFn = OnReadOpen;
  handler.ReadLineFn = OnReadLine;
  handler.WriteAllFn = OnWriteAll;

  auto* context = ImGui::GetCurrentContext();
  Expects(context != nullptr);

  context->SettingsHandlers.push_back(handler);
}

///
void Globals::OnReadLine(ImGuiContext* /*unused*/,
                         ImGuiSettingsHandler* /*unused*/, void* /*unused*/,
                         const char* line) {
  auto json = crude_json::value::parse(line);
  std::cout << "Parsed: " << json.dump() << "\n";
}

///
void Globals::OnWriteAll(ImGuiContext* /*unused*/,
                         ImGuiSettingsHandler* handler,
                         ImGuiTextBuffer* buffer) {
  Expects(handler != nullptr);
  Expects(buffer != nullptr);

  buffer->appendf("[%s][Globals]\n", handler->TypeName);

  const auto& instance = GetInstance();

  for (const auto& setting : instance.settings_) {
    auto json = crude_json::value{};
    std::visit([&json, &setting](const auto& v) { json[setting.first] = v; },
               setting.second);
    buffer->appendf("%s\n", json.dump().c_str());
  }
}
}  // namespace vh::ponc