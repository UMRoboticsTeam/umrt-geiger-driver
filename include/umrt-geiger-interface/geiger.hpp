#ifndef GEIGER_HPP
#define GEIGER_HPP

#include <atomic>
#include <boost/log/trivial.hpp>
#include <boost/signals2/signal.hpp>
#include <cerrno>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <termios.h>
#include <unistd.h>


class Geiger {

public:
    Geiger(const std::string& addr, unsigned int baud_rate);
    Geiger(const Geiger&) = delete;
    Geiger& operator=(const Geiger&) = delete;

    ~Geiger();
    boost::signals2::signal<void(double)> geiger_data;
    void read_geiger(size_t buff_size);
    void stop_read_geiger();

private:
    int fd{ -1 };
    std::atomic<bool> read_mode = true;
    struct termios termios_struct;
};


#endif
