#ifndef VH_ESC_AUTO_INCREMENTABLE_H_
#define VH_ESC_AUTO_INCREMENTABLE_H_

namespace esc {
class AutoIncrementable {
 public:
  explicit AutoIncrementable(int initial_value);

  auto GetNext [[nodiscard]] () -> int;

 private:
  int value_{};
};
}  // namespace esc

#endif  // VH_ESC_AUTO_INCREMENTABLE_H_
