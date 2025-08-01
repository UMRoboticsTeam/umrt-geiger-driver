#include "geiger.hpp"
#include <boost/log/trivial.hpp>
#include <cstring>
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  std::string addr{"/dev/ttyACM0"};
  size_t buff_size{1000};
  if (argc > 1 && sizeof(argv[1]) > 0) {
    if (sizeof(argv[1]) > 0) {
      addr = argv[1];
    }
    if (argc > 2) {
      buff_size = strtol(argv[2], NULL, 10);
    }
  }

  Geiger geiger_instance{addr};
  geiger_instance.geiger_data.connect(
      [](double value) { BOOST_LOG_TRIVIAL(info) << value; });
  std::thread read_thread([&geiger_instance, &buff_size]() {
    geiger_instance.read_geiger(buff_size);
  });

  read_thread.join();
}
