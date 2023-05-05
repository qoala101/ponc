#ifndef VH_PONC_DRAW_RECENT_LOG_H_
#define VH_PONC_DRAW_RECENT_LOG_H_

#include "coreui_log.h"
#include "draw_log_view.h"

namespace vh::ponc::draw {
///
void DrawRecentLog(const coreui::Log &log, const LogView &log_view);
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_RECENT_LOG_H_
