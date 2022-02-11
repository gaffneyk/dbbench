#include "dbbench/benchmarks/tatp.hpp"
#include "dbbench/runner.hpp"

#include <iostream>

struct TATPStdoutRecordVisitor {
  void operator()(const dbbench::tatp::SubscriberRecord &r) {
    std::cout << "SubscriberRecord(" << r.s_id << "," << r.sub_nbr << ",";
    for (bool bit_i : r.bit) {
      std::cout << bit_i << ",";
    }
    for (int hex_i : r.hex) {
      std::cout << hex_i << ",";
    }
    for (int byte2_i : r.byte2) {
      std::cout << byte2_i << ",";
    }
    std::cout << r.msc_location << "," << r.vlr_location << ")" << std::endl;
  }

  void operator()(const dbbench::tatp::AccessInfoRecord &r) {
    std::cout << "AccessInfoRecord(" << r.s_id << "," << +r.ai_type << ","
              << +r.data1 << "," << +r.data2 << "," << r.data3 << "," << r.data4
              << ")" << std::endl;
  }

  void operator()(const dbbench::tatp::SpecialFacilityRecord &r) {
    std::cout << "SpecialFacilityRecord(" << r.s_id << "," << +r.sf_type << ","
              << r.is_active << "," << +r.error_cntrl << "," << +r.data_a << ","
              << r.data_b << ")" << std::endl;
  }

  void operator()(const dbbench::tatp::CallForwardingRecord &r) {
    std::cout << "CallForwardingRecord(" << r.s_id << "," << +r.sf_type << ","
              << +r.start_time << "," << +r.end_time << "," << r.numberx << ")"
              << std::endl;
  }
};

struct TATPStdoutProcedureVisitor {
  void operator()(const dbbench::tatp::GetSubscriberData &p) {
    std::cout << "GetSubscriberData(" << p.s_id << ")" << std::endl;
  }

  void operator()(const dbbench::tatp::GetNewDestination &p) {
    std::cout << "GetNewDestination(" << p.s_id << "," << +p.sf_type << ","
              << +p.start_time << "," << +p.end_time << ")" << std::endl;
  }

  void operator()(const dbbench::tatp::GetAccessData &p) {
    std::cout << "GetAccessData(" << p.s_id << "," << +p.ai_type << ")"
              << std::endl;
  }

  void operator()(const dbbench::tatp::UpdateSubscriberData &p) {
    std::cout << "UpdateSubscriberData(" << p.s_id << "," << p.bit_1 << ","
              << +p.sf_type << "," << +p.data_a << ")" << std::endl;
  }

  void operator()(const dbbench::tatp::UpdateLocation &p) {
    std::cout << "UpdateLocation(" << p.sub_nbr << "," << p.vlr_location << ")"
              << std::endl;
  }

  void operator()(const dbbench::tatp::InsertCallForwarding &p) {
    std::cout << "InsertCallForwarding(" << p.sub_nbr << "," << +p.sf_type
              << "," << +p.start_time << "," << +p.end_time << "," << p.numberx
              << ")" << std::endl;
  }

  void operator()(const dbbench::tatp::DeleteCallForwarding &p) {
    std::cout << "DeleteCallForwarding(" << p.sub_nbr << "," << +p.sf_type
              << "," << +p.start_time << ")" << std::endl;
  }
};

class TATPStdoutWorker {
public:
  explicit TATPStdoutWorker(size_t n_subscriber_records)
      : procedure_generator_(n_subscriber_records) {}

  bool operator()() {
    std::visit(TATPStdoutProcedureVisitor(), procedure_generator_.next());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
  }

private:
  dbbench::tatp::ProcedureGenerator procedure_generator_;
};

int main() {
  uint64_t n_subscriber_records = 10;
  size_t s_warmup = 2;
  size_t s_measure = 5;

  dbbench::tatp::RecordGenerator record_generator(n_subscriber_records);
  while (auto record = record_generator.next()) {
    std::visit(TATPStdoutRecordVisitor(), *record);
  }

  std::vector<TATPStdoutWorker> workers = {
      TATPStdoutWorker(n_subscriber_records)};
  double throughput = dbbench::run(workers, s_warmup, s_measure);

  std::cout << "Throughput: " << throughput << std::endl;

  return 0;
}
