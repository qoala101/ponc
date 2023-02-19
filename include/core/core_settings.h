#ifndef VH_CORE_SETTINGS_H_
#define VH_CORE_SETTINGS_H_

namespace esc::core {
struct Settings {
  float min{-27.F};
  float low{-22.F};
  float high{-18.F};
  float max{6.F};
};
}  // namespace esc::core

#endif  // VH_CORE_SETTINGS_H_
