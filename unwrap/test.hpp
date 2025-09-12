#pragma once
#include <stdlib.h>

bool success() noexcept;

constexpr bool constexpr_success() noexcept {
  return true;
}

consteval bool consteval_success() noexcept {
  return true;
}

struct unwrap_test {
  void(* func)(){nullptr};
  unwrap_test* next{nullptr};
};

extern unwrap_test* unwrap_tests;

#ifndef UNWRAP_TEST
#define UNWRAP_TEST_CONCAT_INNER(a, b) a ## b
#define UNWRAP_TEST_CONCAT(a, b) UNWRAP_TEST_CONCAT_INNER(a, b)
#define UNWRAP_TEST(func) static auto UNWRAP_TEST_CONCAT(unwrap_test_, __COUNTER__) = [] { \
  static unwrap_test test{[] func, unwrap_tests}; \
  unwrap_tests = &test; \
  return __COUNTER__; \
}();
#endif

#ifndef UNWRAP_TEST_LIBRARY
#include <concepts>
#include <source_location>
#include <type_traits>
#include <cstdint>
#include <cstdio>

#if defined(__GNUC__) || defined(_HAS_EXCEPTIONS)
#include <exception>
#endif

#ifndef ASSERT
#define ASSERT(expr) if (!(expr)) { \
  std::printf("[%s:%d] assertion failed\n", __FILE_NAME__, __LINE__); \
  std::fflush(stdout); \
  std::abort(); \
}
#endif

namespace ice {

class source_location {
public:
  consteval source_location() noexcept = default;

  consteval source_location(std::source_location location) noexcept :
    value_(encode(location.file_name(), location.line())) {}

  constexpr std::uint16_t file() const noexcept {
    return static_cast<std::uint16_t>(value_ & 0xFFF);
  }

  constexpr std::uint16_t line() const noexcept {
    return static_cast<std::uint16_t>(value_ >> 12 & 0x7FF);
  }

  constexpr std::uint32_t value() const noexcept {
    return value_;
  }

private:
  friend class error;

  constexpr explicit source_location(std::uint32_t value) noexcept : value_(value) {}

  static consteval std::uint32_t encode(const char* file, std::uint32_t line) noexcept {
    std::uint16_t crc = 0;
    for (auto c = file; *c; c++) {
      crc ^= static_cast<std::uint16_t>(*c) << 4;
      for (int i = 0; i < 8; ++i) {
        if (crc & 0x800) {
          crc = (crc << 1) ^ 0xF33;
        } else {
          crc <<= 1;
        }
      }
    }
    crc &= 0xFFF;

    if (line > 0x7FF - 2) {
#ifdef __has_builtin
#if __has_builtin(__builtin_unreachable)
      __builtin_unreachable();  // error: source location line number is out of range
#endif
#endif
      line = 0x7FF;
    }

    return static_cast<std::uint64_t>(line) << 12 | static_cast<std::uint64_t>(crc);
  }

  std::uint32_t value_{};
};

enum class errc : std::uint8_t {
  success = 0,
  invalid_result_state,
  static_assert_test,
};

enum class error_type : std::uint8_t {};

consteval error_type make_error_type(std::uint8_t index) noexcept {
  return static_cast<error_type>(index);
}

template <class T>
concept ErrorCode = requires(T code) {
  // clang-format off
  requires std::is_enum_v<T> && !std::is_convertible_v<T, std::underlying_type_t<T>>;
  requires sizeof(T) <= sizeof(std::uint32_t);
  { get_error_type(code) } -> std::same_as<error_type>;
  // clang-format on
};

template <class T>
concept ErrorValue = requires(T code) {
  requires std::is_integral_v<T> && !std::is_enum_v<T>;
  requires sizeof(T) <= sizeof(std::uint32_t);
};

class error {
public:
  consteval error() noexcept = default;

  template <ErrorCode Code>
  constexpr explicit error(
    Code code,
    ice::source_location source_location = std::source_location::current()) noexcept :
    value_(encode(static_cast<std::uint32_t>(code), get_error_type(code), source_location)) {}

  template <ErrorValue Value>
  constexpr explicit error(
    Value value,
    ice::error_type type,
    ice::source_location source_location = std::source_location::current()) noexcept :
    value_(encode(static_cast<std::uint32_t>(value), type, source_location)) {}

  constexpr ~error() = default;

  constexpr int code() const noexcept {
    return static_cast<int>(static_cast<std::uint32_t>(value_));
  }

  constexpr ice::error_type type() const noexcept {
    return static_cast<error_type>(value_ >> 32 & 0xFF);
  }

  constexpr ice::source_location source_location() const noexcept {
    return ice::source_location{ static_cast<std::uint32_t>(value_ >> 40 & 0xFFFFFFFF) };
  }

  constexpr std::uint64_t value() const noexcept {
    return value_;
  }

  constexpr explicit operator bool() const noexcept {
    return value_ != 0x00000000'00000000;
  }

  friend constexpr bool operator==(error lhs, error rhs) noexcept {
    return (lhs.value_ & cmp) == (rhs.value_ & cmp);
  }

  friend constexpr bool operator==(error error, error_type type) noexcept {
    return error.type() == type;
  }

  template <ErrorCode Code>
  friend constexpr bool operator==(error error, Code code) noexcept {
    return error == ice::error{ code, ice::source_location{} };
  }

private:
  // 10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000
  static constexpr std::uint64_t bit = 0x80000000'00000000;

  // 10000000'00000000'00000000'11111111'11111111'11111111'11111111'11111111
  static constexpr std::uint64_t cmp = 0x800000FF'FFFFFFFF;

  // 01111111'11111111'11111111'00000000'00000000'00000000'00000000'00000000
  static constexpr std::uint64_t src = 0x7FFFFF00'00000000;

  template <class T>
  friend class result;

  constexpr explicit error(std::uint64_t value) noexcept : value_(value) {}

  static constexpr std::uint64_t encode(
    std::uint64_t code,
    ice::error_type type,
    ice::source_location source_location) noexcept {
    return bit | static_cast<std::uint64_t>(source_location.value()) << 40 |
           static_cast<std::uint64_t>(type) << 32 | code;
  }

  //  1 bits [0 -     1] error state (indicates that the union holds an error)
  // 11 bits [0 - 2'047] line number (source location starts at line 1)
  // 12 bits [0 - 4'095] file path encoded as CRC-12-UMTS (0xF33)
  //  8 bits [0 -   255] error type (index to a global table of pointers)
  // 32 bits [0 - 2³²-1] error code (formatted by error type)
  std::uint64_t value_{};
};

static_assert(!ice::error{});

struct error_category {
  error_category(error_type type, source_location code = std::source_location::current()) noexcept {}

  error_category(error_category&& other) = delete;
  error_category(const error_category& other) = delete;
  error_category& operator=(error_category&& other) = delete;
  error_category& operator=(const error_category& other) = delete;

  virtual ~error_category() {}
};

constexpr ice::error_type get_error_type(errc) noexcept {
  return ice::make_error_type(0);
}

static_assert(ice::error{ ice::errc::static_assert_test });
static_assert(ice::error{ ice::errc::static_assert_test } == ice::errc::static_assert_test);

template <class T>
concept ResultValue = !std::same_as<std::decay_t<T>, ice::error>;

template <class T>
class result final {
  static_assert(ResultValue<T>);

public:
  using value_type = std::remove_cvref_t<T>;
  using error_type = ice::error;

  consteval result() noexcept = default;

  constexpr result(error_type error) noexcept : error_(error) {}

  template <ResultValue Value, class... Args>
  constexpr result(Value&& value, Args&&... args) noexcept :
    error_{}, value_{std::forward<Value>(value), std::forward<Args>(args)...} {}

  constexpr result(result&& other) noexcept = default;
  constexpr result(const result& other) noexcept = default;
  constexpr result& operator=(result&& other) noexcept = default;
  constexpr result& operator=(const result& other) noexcept = default;

  constexpr ~result() = default;

  constexpr explicit operator bool() const noexcept {
    return !error_;
  }

  constexpr value_type& operator*() & noexcept {
    return value_;
  }

  constexpr value_type&& operator*() && noexcept {
    return std::move(value_);
  }

  constexpr const value_type& operator*() const & noexcept {
    return value_;
  }

  constexpr error_type error() const noexcept {
    return error_;
  }

private:
  error_type error_{ ice::errc::invalid_result_state };
  value_type value_;
};

static_assert(!ice::result<long>{});
static_assert(ice::result<long>{}.error() == ice::errc::invalid_result_state);

static_assert(!ice::result<long>{ ice::error{ ice::errc::static_assert_test } });
static_assert(ice::result<long>{ ice::error{ ice::errc::static_assert_test } }.error() == ice::errc::static_assert_test);

static_assert(ice::result<long>{ 1 });
static_assert(*ice::result<long>{ 1 } == 1);

template <>
class result<void> final {
public:
  using value_type = void;
  using error_type = ice::error;

  consteval result() noexcept = default;
  constexpr result(error_type error) noexcept : error_(error) {}

  constexpr result(result&& other) noexcept = default;
  constexpr result(const result& other) noexcept = default;
  constexpr result& operator=(result&& other) noexcept = default;
  constexpr result& operator=(const result& other) noexcept = default;

  constexpr ~result() = default;

  constexpr explicit operator bool() const noexcept {
    return !error_;
  }

  constexpr value_type operator*() const noexcept {}

  constexpr error_type error() const noexcept {
    return error_;
  }

private:
  error_type error_;
};

static_assert(ice::result<void>{});

static_assert(!ice::result<void>{ ice::error{ ice::errc::static_assert_test } });
static_assert(ice::result<void>{ ice::error{ ice::errc::static_assert_test } }.error() == ice::errc::static_assert_test);

}  // namespace ice

enum class errc {
  unknown = 0,
  failure = 1,
  success_never_reached = 100,
  failure_never_reached = 101,
  move_not_initialized = 1000,
};

constexpr auto error_type = ice::make_error_type(2);
consteval ice::error_type get_error_type(errc) noexcept { return error_type; }

template <bool Consteval>
class move {
public:
  consteval move() noexcept = default;
  consteval move(int) noexcept : initialized_(true) {}

  constexpr move(move&& other) noexcept : initialized_(other.initialized_) {
    if constexpr (!Consteval) std::puts("move ctor");
  }

  constexpr move(const move& other) noexcept : initialized_(other.initialized_) {
    if constexpr (!Consteval) std::puts("copy ctor");
  }

  constexpr move& operator=(move&& other) noexcept {
    initialized_ = other.initialized_;
    if constexpr (!Consteval) std::puts("move op");
    return *this;
  }

  constexpr move& operator=(const move& other) noexcept {
    initialized_ = other.initialized_;
    if constexpr (!Consteval) std::puts("copy op");
    return *this;
  }

  constexpr bool initialized() const noexcept {
    return initialized_;
  }

private:
  bool initialized_ = false;
};

class consteval_move {
public:
  consteval consteval_move() noexcept = default;
  constexpr consteval_move(int) noexcept : initialized_(true) {}

  constexpr bool initialized() const noexcept { return initialized_; }

private:
  bool initialized_ = false;
};

#endif
