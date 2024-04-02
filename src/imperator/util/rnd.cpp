#include "imperator/util/rnd.h"
#include "imperator/util/log.h"

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
  thread_local pcg32 g = std::invoke(
    [] {
      gen_seed_vals();
      return pcg32(seed_info().seed, seed_info().stream);
    }
  );

  return g;
}
} // namespace internal

seed_data& seed_info() {
  thread_local seed_data s{};
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

  IMPERATOR_LOG_DEBUG("Seed statement: rnd::seed({:#x}, {:#x});", seed_info().seed, seed_info().stream);
}

std::string base58(std::size_t length) {
  static constexpr char B58_ALPHABET[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

  std::string id(length, 0);
  std::generate_n(
    id.begin(),
    length,
    [] {
      return B58_ALPHABET[get<unsigned char>(0x00, 0xff) % 58];
    }
  );

  return id;
}

std::string uuidv4() {
  thread_local uuids::basic_uuid_random_generator gen{internal::generator()};

  return to_string(gen());
}
} // namespace imp::rnd
