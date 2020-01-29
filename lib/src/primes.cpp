#include "../include/primes.h"

PrimesCache Primes::data_ = PrimesCache{};

PrimesCache::PrimesCache() : data_{}, last_checked_{FIRST_SECTOR - 1} {
#ifdef DEBUG_MODE
  std::cout << "PrimesCache creating..." << std::endl;
  auto start_time = std::chrono::high_resolution_clock::now();
#endif
  std::vector<bool> tmp(FIRST_SECTOR, true);
  for (uint32_t prime = 2; prime < FIRST_SECTOR; ++prime) {
    if (!tmp[prime]) {
      continue;
    }
    data_.push_back(prime);
    for (uint32_t not_prime = prime * prime; not_prime < FIRST_SECTOR;
         not_prime += prime) {
      tmp[not_prime] = false;
    }
  }
#ifdef DEBUG_MODE
  auto end_time = std::chrono::high_resolution_clock::now();
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                    start_time)
                  .count();
  std::cout << "PrimesCache creating finished in " << diff << "ms" << std::endl;
#endif
}

void PrimesCache::add_primes() {
#ifdef DEBUG_MODE
  std::cout << "Adding primes to PrimesCache..." << std::endl;
  auto start_time = std::chrono::high_resolution_clock::now();
  uint32_t start_size = static_cast<uint32_t>(data_.size());
#endif
  std::vector<bool> tmp(SECTOR_SIZE, true);
  uint32_t tmp_size = (UINT32_MAX - last_checked_ > SECTOR_SIZE)
                          ? SECTOR_SIZE
                          : UINT32_MAX - last_checked_;
  for (uint32_t p : data_) {
    uint32_t not_p = p + p * (last_checked_ / p) - last_checked_ - 1;
    if (not_p >= tmp_size) {
      break;
    }
    for (; not_p < tmp_size; not_p += p) {
      tmp[not_p] = false;
    }
  }
  for (uint32_t i = 0; i < tmp_size; ++i) {
    if (tmp[i]) {
      data_.push_back(i + last_checked_ + 1);
    }
  }
  last_checked_ += tmp_size;
#ifdef DEBUG_MODE
  uint32_t end_size = static_cast<uint32_t>(data_.size());
  auto end_time = std::chrono::high_resolution_clock::now();
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                    start_time)
                  .count();
  std::cout << "Adding primes to PrimesCache finished in " << diff << "ms"
            << '\n'
            << end_size - start_size << " new elements\nlast==" << last_checked_
            << std::endl;
#endif
}

uint32_t PrimesCache::operator[](uint32_t pos) {
  while (data_.size() <= pos && last_checked_ != UINT32_MAX) {
    add_primes();
  }
  if (data_.size() > pos) {
    return data_[pos];
  }
#ifdef DEBUG_MODE
  std::cout << "Out of bounds in PrimesCache: operator[](" << pos << ')'
            << std::endl;
#endif
  return 0;
}

uint32_t PrimesCache::operator()(uint32_t pos) const noexcept {
  if (data_.size() > pos) {
    return data_[pos];
  }
  uint32_t left = static_cast<uint32_t>(pos + 1 - data_.size());
  uint32_t now = last_checked_;
  while (left) {
    bool is_prime = false;
    while (!is_prime) {
      if (++now == UINT32_MAX) {
#ifdef DEBUG_MODE
        std::cout << "Out of bounds in PrimesCache: operator()(" << pos << ')'
                  << std::endl;
#endif
        return 0;
      }
      is_prime = true;
      for (uint32_t pr : data_) {
        if (now % pr == 0) {
          is_prime = false;
          break;
        }
      }
      if (is_prime) {
        for (uint32_t i = data_.back() + 2; i * i <= now && i < UINT32_MAX_SQRT;
             i += 2) {
          if (now % i == 0) {
            is_prime = false;
            break;
          }
        }
      }
    }
    --left;
  }
  return now;
}

PrimesCache::const_iterator PrimesCache::begin() const noexcept {
  return data_.begin();
}

PrimesCache::const_iterator PrimesCache::end() const noexcept {
  return data_.end();
}

uint32_t PrimesCache::last_checked() const noexcept { return last_checked_; }

uint32_t PrimesCache::size() const noexcept {
  return static_cast<uint32_t>(data_.size());
}

Primes::Primes() : size_{0}, unbound_{true} {}

Primes::Primes(uint32_t max_value) : size_{0}, unbound_{false} {
  while (data_.last_checked() < max_value) {
    data_.add_primes();
  }
  auto end = std::upper_bound(data_.begin(), data_.end(), max_value);
  size_ = static_cast<uint32_t>(end - data_.begin());
}

uint32_t Primes::operator[](uint32_t pos) {
  if (unbound_ || size_ > pos) {
    return data_[pos];
  }
#ifdef DEBUG_MODE
  std::cout << "Out of bounds in Primes: operator[](" << pos << ')'
            << std::endl;
#endif
  return 0;
}

uint32_t Primes::operator()(uint32_t pos) const noexcept {
  if (unbound_ || size_ > pos) {
    return data_(pos);
  }
#ifdef DEBUG_MODE
  std::cout << "Out of bounds in Primes: operator()(" << pos << ')'
            << std::endl;
#endif
  return 0;
}

uint32_t Primes::size() const noexcept {
  return unbound_ ? data_.size() : size_;
}

Primes::Iterator::Iterator(Primes *owner, uint32_t pos, bool end_it) noexcept
    : owner_{owner}, pos_{pos}, end_it_{end_it} {}

Primes::Iterator Primes::begin() { return Iterator(this); }

Primes::Iterator Primes::end() { return Iterator(this, size_, unbound_); }

Primes::Iterator &
Primes::Iterator::operator+=(Primes::Iterator::difference_type diff) noexcept {
  if (diff < 0) {
    pos_ -= static_cast<uint32_t>(-diff);
  } else {
    pos_ += static_cast<uint32_t>(diff);
  }
  return *this;
}

Primes::Iterator &
Primes::Iterator::operator-=(Primes::Iterator::difference_type diff) noexcept {
  return *this += -diff;
}

Primes::Iterator operator+(const Primes::Iterator &it,
                           Primes::Iterator::difference_type diff) noexcept {
  return Primes::Iterator(it) += diff;
}

Primes::Iterator operator-(const Primes::Iterator &it,
                           Primes::Iterator::difference_type diff) noexcept {
  return Primes::Iterator(it) -= diff;
}

Primes::Iterator operator+(Primes::Iterator::difference_type diff,
                           const Primes::Iterator &it) noexcept {
  return it + diff;
}

Primes::Iterator::difference_type
operator-(const Primes::Iterator &lhs, const Primes::Iterator &rhs) noexcept {
  if (lhs.end_it_ && rhs.end_it_) {
    return 0;
  }
  if (lhs.end_it_ || rhs.end_it_) {
    return lhs.end_it_ ? INT32_MAX : INT32_MIN;
  }
  return lhs.pos_ >= rhs.pos_ ? static_cast<Primes::Iterator::difference_type>(
                                    lhs.pos_ - rhs.pos_)
                              : -static_cast<Primes::Iterator::difference_type>(
                                    rhs.pos_ - lhs.pos_);
}

bool operator==(const Primes::Iterator &lhs,
                const Primes::Iterator &rhs) noexcept {
  return (lhs.end_it_ || rhs.end_it_) ? (lhs.end_it_ && rhs.end_it_)
                                      : lhs.pos_ == rhs.pos_;
}

bool operator!=(const Primes::Iterator &lhs,
                const Primes::Iterator &rhs) noexcept {
  return !(lhs == rhs);
}

bool operator<(const Primes::Iterator &lhs,
               const Primes::Iterator &rhs) noexcept {
  if (lhs.end_it_ && rhs.end_it_) {
    return false;
  }
  if (lhs.end_it_ || rhs.end_it_) {
    return rhs.end_it_;
  }
  return lhs.pos_ < rhs.pos_;
}

bool operator>(const Primes::Iterator &lhs,
               const Primes::Iterator &rhs) noexcept {
  return rhs < lhs;
}

bool operator>=(const Primes::Iterator &lhs,
                const Primes::Iterator &rhs) noexcept {
  return !(lhs < rhs);
}

bool operator<=(const Primes::Iterator &lhs,
                const Primes::Iterator &rhs) noexcept {
  return !(lhs > rhs);
}

Primes::Iterator &Primes::Iterator::operator++() noexcept {
  ++pos_;
  return *this;
}

Primes::Iterator Primes::Iterator::operator++(int) const noexcept {
  Iterator tmp(*this);
  ++tmp.pos_;
  return tmp;
}

Primes::Iterator &Primes::Iterator::operator--() noexcept {
  --pos_;
  return *this;
}

Primes::Iterator Primes::Iterator::operator--(int) const noexcept {
  Iterator tmp(*this);
  --tmp.pos_;
  return tmp;
}

uint32_t Primes::Iterator::operator*() const {
  return end_it_ ? 0 : owner_->operator[](pos_);
}
