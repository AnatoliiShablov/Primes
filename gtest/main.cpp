#include "../lib/include/primes.h"
#include "gtest/gtest.h"
#include <algorithm>

#define FULL_TEST_MODE_OFF

#ifndef FULL_TEST_MODE
static const uint32_t MAX_NUMBER{50000000};
#else
static const uint32_t MAX_NUMBER{UINT32_MAX};
#endif

static std::vector<uint32_t> real_primes;

int main(int argc, char *argv[]) {
  std::cout << "0%" << std::endl;
  uint32_t pr = 0;
  for (uint32_t i = 2;; ++i) {
    bool is_good = true;
    for (uint32_t pr : real_primes) {
      if (pr * pr > i) {
        break;
      }
      if (i % pr == 0) {
        is_good = false;
        break;
      }
    }
    if (is_good) {
      real_primes.push_back(i);
    }
    if (UINT64_C(100) * i / MAX_NUMBER > pr) {
      pr = UINT64_C(100) * i / MAX_NUMBER;
      std::cout << pr << "%" << std::endl;
    }
    if (i >= MAX_NUMBER) {
      break;
    }
  }
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(PrimesCache_by_pos, precalc) {
  PrimesCache obj;
  for (uint32_t i = 0; i < static_cast<uint32_t>(real_primes.size()); ++i) {
    EXPECT_EQ(obj[i], real_primes[i]);
  }
}

TEST(PrimesCache_by_pos, intime) {
  PrimesCache obj;
  for (uint32_t i = 0;
       i < (5000 < real_primes.size() ? 5000 : real_primes.size()); ++i) {
    EXPECT_EQ(obj(i), real_primes[i]);
  }
}

TEST(Primes_WITHOUT_STATIC, by_pos) {
  Primes obj;
  for (uint32_t i = 0;
       i < (5000 < real_primes.size() ? 5000 : real_primes.size()); ++i) {
    EXPECT_EQ(obj(i), real_primes[i]);
  }
}
TEST(Primes_WITHOUT_STATIC, const_access_size_check) {
  Primes obj;
  uint32_t size = obj.size();
  for (uint32_t i = 0; i < 10000; ++i) {
    obj(i);
    EXPECT_EQ(obj.size(), size);
  }
}

TEST(Primes_WITH_STATIC, size) {
  for (uint32_t max = 0; max < 10000; ++max) {
    Primes obj(max);
    auto end = std::upper_bound(real_primes.begin(), real_primes.end(), max);
    if (end == real_primes.end()) {
      break;
    }
    uint32_t exp_size = static_cast<uint32_t>(end - real_primes.begin());
    EXPECT_EQ(obj.size(), exp_size);
  }
}

TEST(Primes_WITH_STATIC, zero_test) {
  Primes obj_bounded(1000);
  EXPECT_EQ(obj_bounded[10000], 0);
  Primes obj_unbounded;
  EXPECT_EQ(obj_unbounded[1000000000], 0);
}

TEST(Primes_WITH_STATIC, by_pos) {
  Primes obj;
  for (uint32_t i = 0; i < static_cast<uint32_t>(real_primes.size()); ++i) {
    EXPECT_EQ(obj[i], real_primes[i]);
  }
}

TEST(Primes_by_iterator, zero_sized) {
  Primes obj(1);
  std::vector<uint32_t> primes;
  for (auto it = obj.begin(); it < obj.end(); ++it) {
    primes.push_back(*it);
  }
  EXPECT_EQ(primes.size(), 0);
}

TEST(Primes_by_iterator, max_is_3000) {
  Primes obj(3000);
  std::vector<uint32_t> primes;
  for (auto it = obj.begin(); it < obj.end(); ++it) {
    primes.push_back(*it);
  }
  EXPECT_EQ(obj.size(), primes.size());
}

TEST(Primes_by_iterator, unbounded) {
  Primes obj;
  std::vector<uint32_t> primes;
  for (auto it = obj.begin(); it < obj.end() && *it != 0; ++it) {
    primes.push_back(*it);
  }
#ifdef FULL_TEST_MODE
  EXPECT_EQ(obj.size(), real_primes.size());
#endif
  EXPECT_EQ(obj.size(), primes.size());
  for (uint32_t i = 0; i < static_cast<uint32_t>(real_primes.size()); ++i) {
    EXPECT_EQ(primes[i], real_primes[i]);
  }
}

TEST(Primes_by_iterator, random_access) {
  Primes obj;
  std::vector<uint32_t> primes;
  for (auto it = obj.begin(); it < obj.end() && *it != 0; it += 1200000) {
    primes.push_back(*it);
  }
  for (uint32_t i = 0; i < static_cast<uint32_t>(primes.size()) &&
                       i * 1200000 < static_cast<uint32_t>(real_primes.size());
       ++i) {
    EXPECT_EQ(primes[i], real_primes[i * 1200000]);
  }
}

TEST(Primes_iterators_test, general) {
  Primes obj;
  auto begin = obj.begin();
  auto end = obj.end();
  EXPECT_LT(begin, end);
  EXPECT_LT(begin, begin + 1);
  auto begin1 = begin;
  ++begin1;
  EXPECT_GT(begin1, begin);
  EXPECT_GT(begin1, begin1 - 1);
  EXPECT_EQ(end, end + 100);
  EXPECT_EQ(end, end - 100);
}
