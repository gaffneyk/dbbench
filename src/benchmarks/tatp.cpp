#include "dbbench/benchmarks/tatp.hpp"

#include <cassert>

static std::string leading_zero_pad(uint64_t x) {
  std::string s = std::to_string(x);
  return std::string(15 - s.length(), '0') + s;
}

dbbench::tatp::RecordGenerator::RecordGenerator(uint64_t n_subscriber_records)
    : n_subscriber_records_(n_subscriber_records), s_id_(1) {}

std::optional<dbbench::tatp::Record> dbbench::tatp::RecordGenerator::next() {
  static std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  if (s_id_ > n_subscriber_records_) {
    return {};
  }

  if (queue_.empty()) {
    std::array<bool, 10> bit{};
    for (bool &bit_i : bit) {
      bit_i = gen_.uniform(0, 1);
    }

    std::array<uint8_t, 10> hex{};
    for (uint8_t &hex_i : hex) {
      hex_i = gen_.uniform(0, 15);
    }

    std::array<uint8_t, 10> byte2{};
    for (uint8_t &byte2_i : byte2) {
      byte2_i = gen_.uniform(0, 255);
    }

    queue_.emplace_back(SubscriberRecord{
        .s_id = s_id_,
        .sub_nbr = leading_zero_pad(s_id_),
        .bit = bit,
        .hex = hex,
        .byte2 = byte2,
        .msc_location = gen_.uniform(uint32_t(1), uint32_t(4294967295)),
        .vlr_location = gen_.uniform(uint32_t(1), uint32_t(4294967295))});

    for (uint8_t ai_type :
         gen_.sample<uint8_t>(gen_.uniform(1, 4), {1, 2, 3, 4})) {
      uint8_t data1 = gen_.uniform(0, 255);
      uint8_t data2 = gen_.uniform(0, 255);
      std::string data3 = gen_.string(3, chars);
      std::string data4 = gen_.string(5, chars);
      queue_.emplace_back(
          AccessInfoRecord{s_id_, ai_type, data1, data2, data3, data4});
    }

    for (uint8_t sf_type :
         gen_.sample<uint8_t>(gen_.uniform(1, 4), {1, 2, 3, 4})) {
      bool is_active = gen_.uniform(0, 99) < 85;
      uint8_t error_cntrl = gen_.uniform(0, 255);
      uint8_t data_a = gen_.uniform(0, 255);
      std::string data_b = gen_.string(5, chars);
      queue_.emplace_back(SpecialFacilityRecord{s_id_, sf_type, is_active,
                                                error_cntrl, data_a, data_b});

      for (uint8_t start_time :
           gen_.sample<uint8_t>(gen_.uniform(0, 3), {0, 8, 16})) {
        uint8_t end_time = start_time + gen_.uniform<uint8_t>(1, 8);
        std::string numberx = gen_.string(15, chars);
        queue_.emplace_back(CallForwardingRecord{s_id_, sf_type, start_time,
                                                 end_time, numberx});
      }
    }

    ++s_id_;
  }

  Record record = std::move(queue_.front());
  queue_.pop_front();

  return record;
}

dbbench::tatp::ProcedureGenerator::ProcedureGenerator(
    uint64_t n_subscriber_records)
    : n_subscriber_records_(n_subscriber_records),
      a_(n_subscriber_records <= 1000000    ? 65535
         : n_subscriber_records <= 10000000 ? 1048575
                                            : 2097151),
      distribution_({35, 10, 35, 2, 14, 2, 2}) {}

dbbench::tatp::Procedure dbbench::tatp::ProcedureGenerator::next() {
  assert(distribution_.min() == 0);
  assert(distribution_.max() == 6);

  int procedure_type = distribution_(gen_.prng());

  switch (procedure_type) {
  case 0:
    return GetSubscriberData{.s_id = generate_s_id()};

  case 1:
    return GetNewDestination{
        .s_id = generate_s_id(),
        .sf_type = gen_.uniform<uint8_t>(1, 4),
        .start_time = (uint8_t)(gen_.uniform(0, 2) * 8),
        .end_time = gen_.uniform<uint8_t>(1, 24),
    };

  case 2:
    return GetAccessData{.s_id = generate_s_id(),
                         .ai_type = gen_.uniform<uint8_t>(1, 4)};

  case 3:
    return UpdateSubscriberData{.s_id = generate_s_id(),
                                .bit_1 = (bool)gen_.uniform(0, 1),
                                .sf_type = gen_.uniform<uint8_t>(1, 4),
                                .data_a = gen_.uniform<uint8_t>()};

  case 4:
    return UpdateLocation{
        .sub_nbr = leading_zero_pad(generate_s_id()),
        .vlr_location = gen_.uniform(uint32_t(1), uint32_t(4294967295ull))};

  case 5:
    return InsertCallForwarding{.sub_nbr = leading_zero_pad(generate_s_id()),
                                .sf_type = gen_.uniform<uint8_t>(1, 4),
                                .start_time = (uint8_t)(gen_.uniform(0, 2) * 8),
                                .end_time = gen_.uniform<uint8_t>(1, 24),
                                .numberx = leading_zero_pad(gen_.uniform(
                                    uint64_t(1), n_subscriber_records_))};

  case 6:
    return DeleteCallForwarding{.sub_nbr = leading_zero_pad(generate_s_id()),
                                .sf_type = gen_.uniform<uint8_t>(1, 4),
                                .start_time =
                                    (uint8_t)(gen_.uniform(0, 2) * 8)};
  default:
    throw std::logic_error("unexpected procedure type: " +
                           std::to_string(procedure_type));
  }
}

uint64_t dbbench::tatp::ProcedureGenerator::generate_s_id() {
  return (gen_.uniform(uint64_t(0), a_) |
          gen_.uniform(uint64_t(1), n_subscriber_records_)) %
             n_subscriber_records_ +
         uint64_t(1);
}
