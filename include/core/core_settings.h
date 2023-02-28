#ifndef VH_CORE_SETTINGS_H_
#define VH_CORE_SETTINGS_H_

namespace esc::core {
///
struct Settings {
  ///
  bool color_flow{};
  ///
  float min_flow{-27.F};
  ///
  float low_flow{-22.F};
  ///
  float high_flow{-18.F};
  ///
  float max_flow{6.F};
};
}  // namespace esc::core

#endif  // VH_CORE_SETTINGS_H_
