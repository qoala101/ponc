#ifndef VH_CORE_PIN_H_
#define VH_CORE_PIN_H_

#include <imgui_node_editor.h>

#include <string>

#include "esc_enums.h"

namespace ne = ax::NodeEditor;

class Node;

class Pin {
 public:
  Pin(ne::PinId id, std::string name, PinKind kind, bool editable);

  Pin(const Pin &) = delete;
  Pin(Pin &&) noexcept = delete;

  auto operator=(const Pin &) noexcept -> Node & = delete;
  auto operator=(Pin &&) noexcept -> Node & = delete;

  virtual ~Pin() = default;

  auto GetId [[nodiscard]] () const -> ne::PinId;
  void SetId(ne::PinId id);

  virtual auto GetType [[nodiscard]] () const -> PinType = 0;

  float value{};

  struct {
    std::string Name{};
    bool editable{};
    Node *node;
    PinKind Kind;
  } ui_data_{};

 private:
  ne::PinId id_;
};

#endif  // VH_CORE_PIN_H_
