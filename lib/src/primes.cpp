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

