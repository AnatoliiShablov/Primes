#include "../lib/include/primes.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>

enum class primes_types : uint32_t { ALL_PRIMES, SUPER_PRIME, MERSENNE };

struct quest {
  uint32_t by_max{100};
  uint32_t by_amount{0};
  primes_types primes_type{primes_types::ALL_PRIMES};
  const char *output_file{nullptr};
  const char *stat_file{nullptr};
};

bool parse_args(quest &spec, int argc, char *argv[]) {
  //  -h --help
  //  -n --amount     // number
  //  -m --max_number // number
  //  -f --file       // file_name
  //  -o --option     // diff types of primes
  //  -s --stat       // file_name
  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "-h") == 0 ||
        std::strcmp(argv[i], "--help") == 0) {
      std::cout
          << "-n --amount     [amount_of_primes]          to set up amount of "
             "printing primes\n"
             "-m --max_number [max_number]                to set up max prime\n"
             "-f --file       [file_name]                 to redirect primes "
             "output to \"file_name\"\n"
             "-o --option     [all|super_simple|mersenne] to set up special "
             "prime's type\n"
             "-s --stat       [file_name]                 to print additional "
             "info to \"file_name\"\n";
      return false;
    }
    if (std::strcmp(argv[i], "-n") == 0 ||
        std::strcmp(argv[i], "--amount") == 0) {
      if (i + 1 < argc) {
        spec.by_amount = static_cast<uint32_t>(std::atoi(argv[++i]));
        spec.by_max = 0;
      } else {
        std::cout << "Wrong amount param" << std::endl;
        return false;
      }
      continue;
    }
    if (std::strcmp(argv[i], "-m") == 0 ||
        std::strcmp(argv[i], "--max_number") == 0) {
      if (i + 1 < argc) {
        spec.by_max = static_cast<uint32_t>(std::atoi(argv[++i]));
        spec.by_amount = 0;
      } else {
        std::cout << "Wrong max number param" << std::endl;
        return false;
      }
      continue;
    }
    if (std::strcmp(argv[i], "-f") == 0 ||
        std::strcmp(argv[i], "--file") == 0) {
      if (i + 1 < argc) {
        spec.output_file = argv[++i];
      } else {
        std::cout << "Wrong file param" << std::endl;
        return false;
      }
      continue;
    }
    if (std::strcmp(argv[i], "-s") == 0 ||
        std::strcmp(argv[i], "--stat") == 0) {
      if (i + 1 < argc) {
        spec.stat_file = argv[++i];
      } else {
        std::cout << "Wrong stat param" << std::endl;
        return false;
      }
      continue;
    }
    if (std::strcmp(argv[i], "-o") == 0 ||
        std::strcmp(argv[i], "--option") == 0) {
      if (i + 1 < argc) {
        ++i;
        if (std::strcmp(argv[i], "all") == 0) {
          spec.primes_type = primes_types::ALL_PRIMES;
          continue;
        }
        if (std::strcmp(argv[i], "super_prime") == 0) {
          spec.primes_type = primes_types::SUPER_PRIME;
          continue;
        }
        if (std::strcmp(argv[i], "mersenne") == 0) {
          spec.primes_type = primes_types::MERSENNE;
          continue;
        }
      }
      std::cout << "Wrong option param" << std::endl;
      return false;
    }
    std::cout << "Wrong param on pos " << i << std::endl;
    return false;
  }
  return true;
}

void write(FILE *output_file, uint32_t prime) {
  if (output_file) {
    std::fprintf(output_file, "%u\n", prime);
  } else {
    std::cout << prime << ' ';
    std::fflush(stdout);
  }
}

int main(int argc, char *argv[]) {

  quest spec{};
  if (!parse_args(spec, argc, argv)) {
    return 0;
  }

  FILE *stat_file = nullptr;
  FILE *output_file = nullptr;

  if (spec.stat_file) {
    stat_file = std::fopen(spec.stat_file, "a");
    if (!stat_file) {
      std::cout << "Can't open stat file" << std::endl;
      return 0;
    }
  }

  if (spec.output_file) {
    output_file = std::fopen(spec.output_file, "w");
    if (!output_file) {
      std::cout << "Can't open output file" << std::endl;
      return 0;
    }
  }

  std::fprintf(stdout,
               "_________________________________\nSpecialization:\n%s -- "
               "%u\noption -- %s\n%s\n_________________________________\n",
               spec.by_amount ? "by amount" : "by max number",
               spec.by_amount + spec.by_max,
               (spec.primes_type == primes_types::ALL_PRIMES)
                   ? "All primes"
                   : ((spec.primes_type == primes_types::SUPER_PRIME)
                          ? "Super simple"
                          : "Mersenne"),
               output_file ? "to file" : "to stdout");
  if (stat_file) {
    std::fprintf(stat_file,
                 "_________________________________\nSpecialization:\n%s -- "
                 "%u\noption -- %s\n%s\n_________________________________\n",
                 spec.by_amount ? "by amount" : "by max number",
                 spec.by_amount + spec.by_max,
                 (spec.primes_type == primes_types::ALL_PRIMES)
                     ? "All primes"
                     : ((spec.primes_type == primes_types::SUPER_PRIME)
                            ? "Super simple"
                            : "Mersenne"),
                 output_file ? "to file" : "to stdout");
  }

  std::vector<uint32_t> primes_only;
  auto checker = [&spec](Primes &obj, uint32_t pos) -> bool {
    if (!obj[pos]) {
      return false;
    }
    switch (spec.primes_type) {
    case primes_types::ALL_PRIMES: {
      return true;
    }
    case primes_types::SUPER_PRIME: {
      auto it_pos = std::lower_bound(
          obj.begin(), obj.begin() + static_cast<int32_t>(pos), pos + 1);
      return it_pos != obj.begin() + static_cast<int32_t>(pos) &&
             *it_pos == pos + 1;
    }
    case primes_types::MERSENNE: {
      return ((obj[pos] + UINT32_C(1)) & obj[pos]) == 0;
    }
    }
    return false;
  };
  std::cout << "Starting..." << std::endl;
  auto start_time = std::chrono::high_resolution_clock::now();
  if (spec.by_amount) {
    Primes obj;
    for (uint32_t i = 0; obj[i] > 0 && primes_only.size() < spec.by_amount;
         ++i) {
      if (checker(obj, i)) {
        primes_only.push_back(obj[i]);
      }
    }
  } else {
    Primes obj(spec.by_max);
    for (uint32_t i = 0; i < obj.size(); ++i) {
      if (checker(obj, i)) {
        primes_only.push_back(obj[i]);
      }
    }
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                    start_time)
                  .count();
  std::cout << "Finished" << std::endl;
  if (output_file) {
    std::for_each(primes_only.begin(), primes_only.end(),
                  [output_file](uint32_t prime) {
                    std::fprintf(output_file, "%u\n", prime);
                  });
  } else {
    std::for_each(primes_only.begin(), primes_only.end(),
                  [](uint32_t prime) { std::cout << prime << ' '; });
    std::cout << std::endl;
  }

  Primes mem_check;
  uint32_t mem_used =
      static_cast<uint32_t>(mem_check.size() + primes_only.size());
  std::fprintf(stdout,
               "Written %u primes in %ld ms\nMemory used (approximately) "
               "%u%s\n_________________________________\n",
               static_cast<uint32_t>(primes_only.size()), diff,
               (mem_used / 262144 ? mem_used / 262144 : mem_used / 256),
               (mem_used / 262144 ? "MB" : "KB"));
  if (stat_file) {
    std::fprintf(stat_file,
                 "Written %u primes in %ld ms\nMemory used (approximately) "
                 "%u%s\n_________________________________\n",
                 static_cast<uint32_t>(primes_only.size()), diff,
                 (mem_used / 262144 ? mem_used / 262144 : mem_used / 256),
                 (mem_used / 262144 ? "MB" : "KB"));
    std::fclose(stat_file);
  }
  if (output_file) {
    std::fclose(output_file);
  }
  return 0;
}
