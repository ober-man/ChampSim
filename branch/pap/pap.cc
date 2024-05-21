#include <array>
#include <bitset>
#include <map>
#include <iostream>

#include "msl/fwcounter.h"
#include "ooo_cpu.h"

namespace pap
{

constexpr std::size_t LOCAL_HISTORY_LENGTH = 14;
constexpr std::size_t COUNTER_BITS = 2;
constexpr std::size_t PAP_HISTORY_TABLE_SIZE = (1 << LOCAL_HISTORY_LENGTH);

std::map<O3_CPU*, std::map<uint64_t, std::bitset<LOCAL_HISTORY_LENGTH>>> local_history;
std::map<O3_CPU*, std::map<uint64_t, std::array<champsim::msl::fwcounter<COUNTER_BITS>, PAP_HISTORY_TABLE_SIZE>>> pattern_history;

} // namespace pap

void O3_CPU::initialize_branch_predictor() { std::cout << "CPU " << cpu << " PAp branch predictor" << std::endl; }

uint8_t O3_CPU::predict_branch(uint64_t ip)
{
  auto pap_hash = pap::local_history[this][ip];
  auto pap_value = pap::pattern_history[this][ip][pap_hash.to_ulong()];
  return pap_value.value() >= (pap_value.maximum / 2);
}

void O3_CPU::last_branch_result(uint64_t ip, uint64_t branch_target, uint8_t taken, uint8_t branch_type)
{
  auto pap_hash = pap::local_history[this][ip];
  if (taken) {
    pap::pattern_history[this][ip][pap_hash.to_ulong()] += 1;
  } else {
    pap::pattern_history[this][ip][pap_hash.to_ulong()] -= 1;
  }

  pap::local_history[this][ip] <<= 1;
  pap::local_history[this][ip][0] = taken;
}