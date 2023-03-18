#ifndef VH_COREUI_FAMILY_H_
#define VH_COREUI_FAMILY_H_

#include <memory>
#include <string>

#include "core_i_family.h"
#include "core_i_node.h"
#include "coreui_node.h"
#include "cpp_safe_ptr.h"

namespace esc::coreui {
///
class Project;

///
class Family {
 public:
  ///
  Family(cpp::SafePtr<Project> parent_project,
         cpp::SafePtr<const core::IFamily> family,
         std::vector<cpp::SafePtr<const Node>> nodes);

  ///
  auto GetFamily() const -> const core::IFamily &;
  ///
  auto GetNodes() const -> const std::vector<cpp::SafePtr<const Node>> &;
  ///
  auto GetLabel() const -> std::string;
  ///
  auto CreateNode() const -> std::unique_ptr<core::INode>;

 private:
  ///
  cpp::SafePtr<Project> parent_project_;
  ///
  cpp::SafePtr<const core::IFamily> family_;
  ///
  std::vector<cpp::SafePtr<const Node>> nodes_{};
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
