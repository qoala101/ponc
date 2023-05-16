#ifndef VH_PONC_CPP_NON_COPYABLE_H_
#define VH_PONC_CPP_NON_COPYABLE_H_

namespace vh::ponc::cpp {
///
class NonCopyable {
 public:
  ///
  NonCopyable(const NonCopyable &) = delete;
  ///
  NonCopyable(NonCopyable &&) noexcept = delete;

  ///
  auto operator=(const NonCopyable &) -> NonCopyable & = delete;
  ///
  auto operator=(NonCopyable &&) noexcept -> NonCopyable & = delete;

  ///
  virtual ~NonCopyable() = default;

 protected:
  ///
  NonCopyable() = default;
};
}  // namespace vh::ponc::cpp

#endif  // VH_PONC_CPP_NON_COPYABLE_H_
