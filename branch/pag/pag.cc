#include <array>
#include <bitset>
#include <map>
#include <iostream>

#include "msl/fwcounter.h"
#include "ooo_cpu.h"

namespace pag
{

constexpr std::size_t LOCAL_HISTORY_LENGTH = 14;
constexpr std::size_t COUNTER_BITS = 2;
constexpr std::size_t PAG_HISTORY_TABLE_SIZE = (1 << LOCAL_HISTORY_LENGTH);

std::map<O3_CPU*, std::map<uint64_t, std::bitset<LOCAL_HISTORY_LENGTH>>> local_history;
std::map<O3_CPU*, std::array<champsim::msl::fwcounter<COUNTER_BITS>, PAG_HISTORY_TABLE_SIZE>> global_history;

} // namespace pag

void O3_CPU::initialize_branch_predictor() { std::cout << "CPU " << cpu << " PAg branch predictor" << std::endl; }

uint8_t O3_CPU::predict_branch(uint64_t ip)
{
  auto pag_hash = pag::local_history[this][ip];
  auto pag_value = pag::global_history[this][pag_hash.to_ulong()];
  return pag_value.value() >= (pag_value.maximum / 2);
}

void O3_CPU::last_branch_result(uint64_t ip, uint64_t branch_target, uint8_t taken, uint8_t branch_type)
{
  auto pag_hash = pag::local_history[this][ip];
  if (taken) {
    pag::global_history[this][pag_hash.to_ulong()] += 1;
  } else {
    pag::global_history[this][pag_hash.to_ulong()] -= 1;
  }

  pag::local_history[this][ip] <<= 1;
  pag::local_history[this][ip][0] = taken;
}