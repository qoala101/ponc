/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_NODE_REPLACER_H_
#define VH_PONC_COREUI_NODE_REPLACER_H_

#include "core_i_node.h"
#include "coreui_event.h"
#include "cpp_safe_ptr.h"

namespace vh::ponc::coreui {
///
class Project;

///
class NodeReplacer {
 public:
  ///
  explicit NodeReplacer(cpp::SafePtr<Project> parent_project);

  ///
  auto CanReplaceNode(const core::INode &source_node,
                      const core::INode &target_node) const -> bool;
  ///
  auto ReplaceNode(const core::INode &source_node,
                   const std::vector<ne::PinId> &source_output_pins,
                   std::unique_ptr<core::INode> target_node,
                   std::vector<core::UnspecifiedIdValue> reusable_ids) const
      -> Event &;

 private:
  ///
  auto RewireUsedIds(const core::INode &source_node,
                     const std::vector<ne::PinId> &source_output_pins,
                     core::INode &target_node) const;
  ///
  auto GetFreeIds(std::vector<core::UnspecifiedIdValue> source_ids,
                  std::vector<core::UnspecifiedIdValue> target_ids,
                  std::vector<core::UnspecifiedIdValue> reusable_ids) const;
  ///
  void ReuseSourceIds(std::vector<core::UnspecifiedIdValue> source_ids,
                      std::vector<core::UnspecifiedIdValue> target_ids,
                      std::vector<core::UnspecifiedIdValue> reusable_ids,
                      const std::vector<ne::PinId *> &ids_to_generate) const;

  ///
  cpp::SafePtr<Project> parent_project_;
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_NODE_REPLACER_H_
