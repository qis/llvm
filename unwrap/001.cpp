#include "test.hpp"

ice::result<void> void_success() noexcept {
  if (!success())
    return ice::error{ errc::failure };
  return {};
}

ice::result<void> void_failure() noexcept {
  if (success())
    return ice::error{ errc::failure };
  return {};
}

ice::result<long> long_success() noexcept {
  if (!success())
    return ice::error{ errc::failure };
  return 33;
}

ice::result<long> long_failure() noexcept {
  if (success())
    return ice::error{ errc::failure };
  return 33;
}

UNWRAP_TEST({
  ASSERT(void_success());
  ASSERT(!void_failure());
  ASSERT(long_success());
  ASSERT(*long_success() == 33);
  ASSERT(!long_failure());
});

// ------------------------------------------------------------------------------------------------

constexpr ice::result<void> void_constexpr_success() noexcept {
  return {};
}

constexpr ice::result<void> void_constexpr_failure() noexcept {
  return ice::error{ errc::failure };
}

constexpr ice::result<long> long_constexpr_success() noexcept {
  return 33;
}

constexpr ice::result<long> long_constexpr_failure() noexcept {
  return ice::error{ errc::failure };
}

static_assert(void_constexpr_success());
static_assert(!void_constexpr_failure());
static_assert(long_constexpr_success());
static_assert(*long_constexpr_success() == 33);
static_assert(!long_constexpr_failure());

UNWRAP_TEST({
  ASSERT(void_constexpr_success());
  ASSERT(!void_constexpr_failure());
  ASSERT(long_constexpr_success());
  ASSERT(*long_constexpr_success() == 33);
  ASSERT(!long_constexpr_failure());
});

// ------------------------------------------------------------------------------------------------

consteval ice::result<void> void_consteval_success() noexcept {
  return {};
}

consteval ice::result<void> void_consteval_failure() noexcept {
  return ice::error{ errc::failure };
}

consteval ice::result<long> long_consteval_success() noexcept {
  return 33;
}

consteval ice::result<long> long_consteval_failure() noexcept {
  return ice::error{ errc::failure };
}

static_assert(void_consteval_success());
static_assert(!void_consteval_failure());
static_assert(long_consteval_success());
static_assert(*long_consteval_success() == 33);
static_assert(!long_consteval_failure());

UNWRAP_TEST({
  ASSERT(void_consteval_success());
  ASSERT(!void_consteval_failure());
  ASSERT(long_consteval_success());
  ASSERT(*long_consteval_success() == 33);
  //ASSERT(!long_consteval_failure());
});

// ================================================================================================

ice::result<void> unwrap_void_success() noexcept {
  __unwrap__ void_success();
  return {};
}

ice::result<void> unwrap_void_failure() noexcept {
  __unwrap__ void_failure();
  ASSERT(false);
  return {};
}

ice::result<void> unwrap_long_success() noexcept {
  const auto i = __unwrap__ long_success();
  ASSERT(i == 33);
  return {};
}

ice::result<void> unwrap_long_failure() noexcept {
  const auto i = __unwrap__ long_failure();
  ASSERT(false);
  return {};
}

UNWRAP_TEST({
  ASSERT(unwrap_void_success());
  ASSERT(!unwrap_void_failure());
  ASSERT(unwrap_long_success());
  ASSERT(!unwrap_long_failure());
});

// ------------------------------------------------------------------------------------------------

constexpr ice::result<void> unwrap_void_constexpr_success() noexcept {
  __unwrap__ void_constexpr_success();
  return {};
}

constexpr ice::result<void> unwrap_void_constexpr_failure() noexcept {
  __unwrap__ void_constexpr_failure();
  return {};
}

constexpr ice::result<void> unwrap_long_constexpr_success() noexcept {
  const auto i = __unwrap__ long_constexpr_success();
  //static_assert(i == 33);
  return {};
}

constexpr ice::result<void> unwrap_long_constexpr_failure() noexcept {
  const auto i = __unwrap__ long_constexpr_failure();
  return {};
}

static_assert(unwrap_void_constexpr_success());
//static_assert(!unwrap_void_constexpr_failure());
static_assert(unwrap_long_constexpr_success());
//static_assert(!unwrap_long_constexpr_failure());

UNWRAP_TEST({
  ASSERT(unwrap_void_constexpr_success());
  ASSERT(!unwrap_void_constexpr_failure());
  ASSERT(unwrap_long_constexpr_success());
  ASSERT(!unwrap_long_constexpr_failure());
});

// ------------------------------------------------------------------------------------------------

consteval ice::result<void> unwrap_void_consteval_success() noexcept {
  __unwrap__ void_consteval_success();
  return {};
}

consteval ice::result<void> unwrap_void_consteval_failure() noexcept {
  __unwrap__ void_consteval_failure();
  return {};
}

consteval ice::result<void> unwrap_long_consteval_success() noexcept {
  const auto i = __unwrap__ long_consteval_success();
  //static_assert(i == 33);
  return {};
}

consteval ice::result<void> unwrap_long_consteval_failure() noexcept {
  const auto i = __unwrap__ long_consteval_failure();
  return {};
}

static_assert(unwrap_void_consteval_success());
//static_assert(!unwrap_void_consteval_failure());
static_assert(unwrap_long_consteval_success());
//static_assert(!unwrap_long_consteval_failure());

UNWRAP_TEST({
  ASSERT(unwrap_void_consteval_success());
  //ASSERT(!unwrap_void_consteval_failure());
  //ASSERT(unwrap_long_consteval_success());
  //ASSERT(!unwrap_long_consteval_failure());
});
