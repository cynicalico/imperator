#include "imp/util/rnd.hpp"

//#include "imp/util/log.hpp"

namespace imp::rnd {
void gen_seed_vals() {
  std::uint64_t seed_vals[2];
  auto seed_seq = pcg_extras::seed_seq_from<std::random_device>();
  pcg_extras::generate_to<2>(seed_seq, seed_vals);

  seed_info().seed = seed_vals[0];
  seed_info().stream = seed_vals[1];
}

namespace internal {
pcg32& generator() {
  static thread_local pcg32 g = std::invoke([] {
    gen_seed_vals();
    return pcg32(seed_info().seed, seed_info().stream);
  });

  return g;
}
} // namespace internal

seed_data& seed_info() {
  static thread_local seed_data s{};
  return s;
}

void reseed() {
  internal::generator(); // Make sure the generator has been invoked before
  gen_seed_vals();
  internal::generator().seed(seed_info().seed, seed_info().stream);
}

void seed(std::uint64_t seed, std::uint64_t stream) {
  internal::generator().seed(seed, stream);
  seed_info().seed = seed;
  seed_info().stream = stream;
}

void debug_show_seed() {
  internal::generator(); // Make sure generator has been invoked before

  //  if (std::uint64_t(seed_info().seed >> 64) == 0 && std::uint64_t(seed_info().stream >> 64) == 0) {
  //    IMP_LOG_DEBUG(
  //        "Seed statement: rnd::seed({:#x}, {:#x});",
  //        std::uint64_t(seed_info().seed),
  //        std::uint64_t(seed_info().stream)
  //    );
  //
  //  } else {
  //    auto seed_hi = std::uint64_t(seed_info().seed >> 64);
  //    auto seed_lo = std::uint64_t(seed_info().seed);
  //    auto stream_hi = std::uint64_t(seed_info().stream >> 64);
  //    auto stream_lo = std::uint64_t(seed_info().stream);
  //    IMP_LOG_DEBUG(
  //        "Seed statement: rnd::seed128({:#x}, {:#x}, {:#x}, {:#x});",
  //        seed_hi, seed_lo,
  //        stream_hi, stream_lo
  //    );
  //  }
}

std::string base58(std::size_t length) {
  const static char B58_ALPHABET[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

  std::string id(length, 0);
  std::generate_n(id.begin(), length, [] {
    return B58_ALPHABET[get<unsigned char>(0x00, 0xff) % 58];
  });

  return id;
}

std::string uuidv4() {
  static thread_local uuids::basic_uuid_random_generator gen{internal::generator()};

  return uuids::to_string(gen());
}
} // namespace imp::rnd
