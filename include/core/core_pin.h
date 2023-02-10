#ifndef VH_CORE_PIN_H_
#define VH_CORE_PIN_H_

#include <imgui_node_editor.h>

#include <string>

#include "esc_enums.h"

namespace ne = ax::NodeEditor;

class INode;

class IPin {
 public:
  IPin(ne::PinId id, std::string name, PinKind kind, bool editable);

  IPin(const IPin &) = delete;
  IPin(IPin &&) noexcept = delete;

  auto operator=(const IPin &) noexcept -> INode & = delete;
  auto operator=(IPin &&) noexcept -> INode & = delete;

  virtual ~IPin() = default;

  auto GetId  () const -> ne::PinId;
  void SetId(ne::PinId id);

  virtual auto GetType  () const -> PinType = 0;

  struct {
    std::string Name{};
    bool editable{};
    INode *node;
    PinKind Kind;
  } ui_data_{};

 private:
  ne::PinId id_;
};

#endif  // VH_CORE_PIN_H_
