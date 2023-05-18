/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_LOG_VIEW_H_
#define VH_PONC_DRAW_LOG_VIEW_H_

#include <string>
#include <vector>

#include "coreui_log.h"
#include "draw_i_view.h"

namespace vh::ponc::draw {
///
class LogView : public IView {
 public:
  ///
  static void DrawMessages(const std::vector<coreui::LogMessage> &messages,
                           bool wrap_text = false);

  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(const coreui::Log &log);
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_LOG_VIEW_H_
