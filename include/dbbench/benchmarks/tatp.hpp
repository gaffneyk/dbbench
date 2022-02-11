#ifndef DBBENCH_TATP_HPP
#define DBBENCH_TATP_HPP

#include "dbbench/generator.hpp"

#include <array>
#include <deque>
#include <optional>
#include <string>
#include <utility>
#include <variant>

namespace dbbench::tatp {

/*
 * Records.
 */

struct SubscriberRecord {
  uint64_t s_id;
  std::string sub_nbr;
  std::array<bool, 10> bit;
  std::array<uint8_t, 10> hex;
  std::array<uint8_t, 10> byte2;
  uint32_t msc_location;
  uint32_t vlr_location;
};

struct AccessInfoRecord {
  uint64_t s_id;
  uint8_t ai_type;
  uint8_t data1;
  uint8_t data2;
  std::string data3;
  std::string data4;
};

struct SpecialFacilityRecord {
  uint64_t s_id;
  uint8_t sf_type;
  bool is_active;
  uint8_t error_cntrl;
  uint8_t data_a;
  std::string data_b;
};

struct CallForwardingRecord {
  uint64_t s_id;
  uint8_t sf_type;
  uint8_t start_time;
  uint8_t end_time;
  std::string numberx;
};

using Record = std::variant<SubscriberRecord, AccessInfoRecord,
                            SpecialFacilityRecord, CallForwardingRecord>;

/*
 * Procedures.
 */

struct GetSubscriberData {
  uint64_t s_id;
};

struct GetNewDestination {
  uint64_t s_id;
  uint8_t sf_type;
  uint8_t start_time;
  uint8_t end_time;
};

struct GetAccessData {
  uint64_t s_id;
  uint8_t ai_type;
};

struct UpdateSubscriberData {
  uint64_t s_id;
  bool bit_1;
  uint8_t sf_type;
  uint8_t data_a;
};

struct UpdateLocation {
  std::string sub_nbr;
  uint32_t vlr_location;
};

struct InsertCallForwarding {
  std::string sub_nbr;
  uint8_t sf_type;
  uint8_t start_time;
  uint8_t end_time;
  std::string numberx;
};

struct DeleteCallForwarding {
  std::string sub_nbr;
  uint8_t sf_type;
  uint8_t start_time;
};

using Procedure =
    std::variant<GetSubscriberData, GetNewDestination, GetAccessData,
                 UpdateSubscriberData, UpdateLocation, InsertCallForwarding,
                 DeleteCallForwarding>;

class RecordGenerator {
public:
  explicit RecordGenerator(uint64_t n_subscriber_records);

  std::optional<Record> next();

private:
  uint64_t n_subscriber_records_;
  uint64_t s_id_;
  std::deque<Record> queue_;
  Generator gen_;
};

class ProcedureGenerator {
public:
  explicit ProcedureGenerator(uint64_t n_subscriber_records);

  Procedure next();

private:
  uint64_t generate_s_id();

  uint64_t n_subscriber_records_;
  uint64_t a_;
  std::discrete_distribution<int> distribution_;
  Generator gen_;
};

} // namespace dbbench::tatp

#endif // DBBENCH_TATP_HPP
