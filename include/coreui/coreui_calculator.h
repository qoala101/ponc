/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_CALCULATOR_H_
#define VH_PONC_COREUI_CALCULATOR_H_

#include <optional>

#include "calc_calculation_task.h"
#include "core_diagram.h"

namespace vh::ponc::coreui {
///
class Project;

///
class Calculator {
 public:
  ///
  explicit Calculator(cpp::SafePtr<Project> parent_project);

  ///
  void OnFrame();
  ///
  void Calculate();
  ///
  void Cancel();
  ///
  auto IsRunning() const -> bool;
  ///
  auto GetProgress() const -> float;

 private:
  ///
  auto ValidateInputs(const std::vector<calc::TreeNode>& input_nodes) const;
  ///
  auto AsFamilyNodes(
      const std::vector<std::unique_ptr<core::IFamily>>& families) const;
  ///
  auto PopulateOutput(const calc::TreeNode& output_tree, ne::PinId output_pin);
  ///
  auto PopulateDiagram(const std::vector<calc::TreeNode>& calculated_trees);
  ///
  auto ValidateResult(
      const std::map<core::IdValue<ne::PinId>, core::IdValue<ne::NodeId>>&
          output_root_ids) const;
  ///
  void LogResult(const std::vector<calc::TreeNode>& calculated_trees,
                 std::string_view diagram_name) const;
  ///
  void ProcessResult(const std::vector<calc::TreeNode>& calculated_trees);

  ///
  cpp::SafePtr<Project> parent_project_;
  ///
  std::optional<core::Diagram> diagram_copy_{};
  ///
  std::optional<calc::CalculationTask> calculation_task_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_CALCULATOR_H_
