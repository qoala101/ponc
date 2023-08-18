#include "app_globals.h"

#include <crude_json.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <iostream>
#include <memory>

#include "cpp_assert.h"
#include "json_global_serializer.h"

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
  const auto json = crude_json::value::parse(line);
  const auto& json_object = json.get<crude_json::object>();

  Expects(!json_object.empty());
  const auto& [name, value] = *json_object.cbegin();

  auto& instance = GetInstance();
  instance.settings_json_[name] = value;
}

///
void Globals::OnWriteAll(ImGuiContext* /*unused*/,
                         ImGuiSettingsHandler* handler,
                         ImGuiTextBuffer* buffer) {
  const auto& instance = GetInstance();

  if (instance.settings_json_.is_null()) {
    return;
  }

  const auto& settings_object =
      instance.settings_json_.get<crude_json::object>();

  if (settings_object.empty()) {
    return;
  }

  Expects(handler != nullptr);
  Expects(buffer != nullptr);

  buffer->appendf("[%s][Globals]\n", handler->TypeName);

  for (const auto& [name, value] : settings_object) {
    auto json = crude_json::value{};
    json[name] = value;

    buffer->appendf("%s\n", json.dump().c_str());
  }
}
}  // namespace vh::ponc