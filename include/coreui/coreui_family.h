/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_FAMILY_H_
#define VH_PONC_COREUI_FAMILY_H_

#include <memory>
#include <string>

#include "core_i_family.h"
#include "core_i_node.h"
#include "coreui_node.h"
#include "cpp_safe_ptr.h"

namespace vh::ponc::coreui {
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
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_FAMILY_H_
