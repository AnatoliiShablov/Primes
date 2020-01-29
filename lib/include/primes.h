#ifndef PRIMES_H
#define PRIMES_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#ifdef DEBUG_MODE
#include <chrono>
#include <iostream>
#endif

namespace {
const uint32_t FIRST_SECTOR{2048};
const uint32_t SECTOR_SIZE{1048576};
const uint32_t UINT32_MAX_SQRT{65536};
} // namespace

class PrimesCache {
public:
  using const_iterator = std::vector<uint32_t>::const_iterator;

  PrimesCache();

  void add_primes();

  uint32_t operator[](uint32_t pos);
  uint32_t operator()(uint32_t pos) const noexcept;

  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;

  uint32_t last_checked() const noexcept;
  uint32_t size() const noexcept;

private:
  std::vector<uint32_t> data_;
  uint32_t last_checked_;
};

class Primes {
public:
  Primes();
  Primes(uint32_t max_value);

  uint32_t operator[](uint32_t pos);
  uint32_t operator()(uint32_t pos) const noexcept;

  uint32_t size() const noexcept;

  class Iterator {
  public:
    using difference_type = int32_t;
    using value_type = uint32_t;
    using pointer = const uint32_t *;
    using reference = const uint32_t &;
    using iterator_category = std::random_access_iterator_tag;

    explicit Iterator(Primes *owner, uint32_t pos = 0,
                      bool end_it = false) noexcept;
    Iterator &operator+=(difference_type diff) noexcept;
    Iterator &operator++() noexcept;
    Iterator operator++(int) const noexcept;
    friend Iterator operator+(Iterator const &it,
                              difference_type diff) noexcept;
    friend Iterator operator+(difference_type diff,
                              Iterator const &it) noexcept;
    Iterator &operator-=(difference_type diff) noexcept;
    Iterator &operator--() noexcept;
    Iterator operator--(int) const noexcept;
    friend Iterator operator-(Iterator const &it,
                              difference_type diff) noexcept;

    friend difference_type operator-(Iterator const &lhs,
                                     Iterator const &rhs) noexcept;

    friend bool operator==(Iterator const &lhs, Iterator const &rhs) noexcept;
    friend bool operator!=(Iterator const &lhs, Iterator const &rhs) noexcept;

    friend bool operator<(Iterator const &lhs, Iterator const &rhs) noexcept;
    friend bool operator>(Iterator const &lhs, Iterator const &rhs) noexcept;
    friend bool operator>=(Iterator const &lhs, Iterator const &rhs) noexcept;
    friend bool operator<=(Iterator const &lhs, Iterator const &rhs) noexcept;

    uint32_t operator*() const;

  private:
    Primes *owner_;
    uint32_t pos_;
    bool end_it_;
  };
  friend Iterator operator+(Iterator const &it,
                            Iterator::difference_type diff) noexcept;
  friend Iterator operator+(Iterator::difference_type diff,
                            Iterator const &it) noexcept;
  friend Iterator operator-(Iterator const &it,
                            Iterator::difference_type diff) noexcept;

  friend Iterator::difference_type operator-(Iterator const &lhs,
                                             Iterator const &rhs) noexcept;

  friend bool operator==(Iterator const &lhs, Iterator const &rhs) noexcept;
  friend bool operator!=(Iterator const &lhs, Iterator const &rhs) noexcept;

  friend bool operator<(Iterator const &lhs, Iterator const &rhs) noexcept;
  friend bool operator>(Iterator const &lhs, Iterator const &rhs) noexcept;
  friend bool operator>=(Iterator const &lhs, Iterator const &rhs) noexcept;
  friend bool operator<=(Iterator const &lhs, Iterator const &rhs) noexcept;

  Iterator begin();
  Iterator end();

private:
  static PrimesCache data_;
  uint32_t size_;
  bool unbound_;
};

#endif // PRIMES_H
