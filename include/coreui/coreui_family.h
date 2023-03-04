#ifndef VH_COREUI_FAMILY_H_
#define VH_COREUI_FAMILY_H_

#include <memory>
#include <string>

#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_link.h"
#include "cpp_callbacks.h"
#include "cpp_safe_ptr.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
///
class Family {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Signal<std::unique_ptr<core::INode>> node_created{};
    ///
    cpp::Signal<const core::Link &> link_created{};
  };

  ///
  Family(cpp::SafePtr<const core::IFamily> family,
         cpp::SafePtr<core::IdGenerator> id_generator, Callbacks callbacks);

  ///
  auto GetLabel() const -> std::string;
  ///
  auto NodesHavePinOfKind(ne::PinKind pin_kind) const -> bool;
  ///
  void CreateNodeAt(const ImVec2 &pos) const;
  ///
  void CreateNodeAtAndConnectTo(const ImVec2 &pos, ne::PinId pin_id,
                                ne::PinKind pin_kind) const;

 private:
  ///
  cpp::SafePtr<const core::IFamily> family_;
  ///
  cpp::SafePtr<core::IdGenerator> id_generator_;
  ///
  Callbacks callbacks_{};
};

///
struct FamilyGroup {
  ///
  std::string label{};
  ///
  std::vector<Family> families{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_FAMILY_H_
