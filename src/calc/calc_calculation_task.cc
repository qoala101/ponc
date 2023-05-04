#include "calc_calculation_task.h"

namespace vh::ponc::calc {
///
CalculationTask::CalculationTask(Calculator::ConstructorArgs args) {
  args.step_callback =
      std::bind_front(&CalculationTask::OnCalculationStep, this);

  task_ = std::async(std::launch::async, [args = std::move(args)]() {
    const auto calculator = calc::Calculator{args};
    return calculator.GetResult();
  });
}

///
CalculationTask::~CalculationTask() { Stop(); }

///
void CalculationTask::Stop() { stop_requested_ = true; }

///
auto CalculationTask::IsRunning() const -> bool {
  if (!task_.valid()) {
    return false;
  }

  const auto calculation_status = task_.wait_for(std::chrono::seconds::zero());
  return calculation_status != std::future_status::ready;
}

///
auto CalculationTask::GetProgress() const -> float { return progress_; }

///
auto CalculationTask::GetResult() -> std::optional<TreeNode> {
  if (!task_.valid() || IsRunning()) {
    return std::nullopt;
  }

  auto result = task_.get();

  task_ = std::future<calc::TreeNode>{};
  stop_requested_ = false;
  progress_ = 0;

  return std::move(result);
}

///
auto CalculationTask::OnCalculationStep(const calc::Calculator &calculator)
    -> calc::Calculator::StepStatus {
  progress_ = calculator.GetProgress();
  return stop_requested_ ? calc::Calculator::StepStatus::kStopCalculation
                         : calc::Calculator::StepStatus::kContinueToNextStep;
}
}  // namespace vh::ponc::calc