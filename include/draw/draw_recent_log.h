#ifndef VH_PONC_DRAW_RECENT_LOG_H_
#define VH_PONC_DRAW_RECENT_LOG_H_

#include "coreui_log.h"

namespace vh::ponc::draw {
///
class RecentLog {
 public:
  ///
  void Draw(const coreui::Log &log);

 private:
  ///
  bool opened_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_RECENT_LOG_H_
