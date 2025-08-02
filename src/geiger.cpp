#include "geiger.hpp"
#include <boost/log/trivial.hpp>
#include <boost/signals2/signal.hpp>
#include <cerrno>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
using std::runtime_error;

Geiger::Geiger(const std::string& addr) {
    fd = open(addr.c_str(), O_NOCTTY | O_RDONLY);
    if (fd == -1) {
        BOOST_LOG_TRIVIAL(error) << "[x] could not open serial file: " << addr << " errno: " << errno;
        throw runtime_error("could not open serial file");
    }

    // get current termios config associated with fd
    if (tcgetattr(fd, &termios_struct) == -1) {
        close(fd);
        fd = -1;
        BOOST_LOG_TRIVIAL(error) << "[x] could not get current termios config: " << addr << " errno: " << errno;
        throw runtime_error("could not get current termios config");
    }
    // apply custom termios config
    termios_struct.c_cflag &= ~(CSTOPB | PARENB | CSIZE); // 1 stop bit and no parity bit
    termios_struct.c_cflag |= (CS8 | CREAD | CLOCAL);     // 8-bit data width, enable receiver, ignore modem controls.
    cfsetospeed(
            &termios_struct,
            B1152000
    );      // set fixed output baud rate to 1152000 per
            // competition specifications
    cfsetispeed(
            &termios_struct,
            B1152000
    );      // set fixed input baud rate to 1152000 per competition
            // specifications
    termios_struct.c_cc[VMIN] = 1;
    termios_struct.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &termios_struct) == -1) { // set configuration immediately
        close(fd);
        fd = -1;
        BOOST_LOG_TRIVIAL(error) << "[x] could not set termios config. errno: " << errno;
        throw runtime_error("could not get current termios config");
    }
    BOOST_LOG_TRIVIAL(info) << "[o] Termios custom config set";
}

Geiger::~Geiger() {
    read_mode = false;
    if (fd >= 0) { close(fd); }
    fd = -1;
}

// read loop for reading geiger counter messages
void Geiger::read_geiger(size_t buff_size) {
    if (fd == -1) {
        BOOST_LOG_TRIVIAL(error) << "[x] serial file descriptor not valid";
        throw runtime_error("serial file descriptor not valid");
    }
    read_mode = 1;
    char read_buffer[buff_size];
    while (read_mode) {
        ssize_t nbytes = read(fd, read_buffer, buff_size);
        if (nbytes == -1) {
            BOOST_LOG_TRIVIAL(error) << "[x] failed to read from device";
        } else {
            double read_value = strtod(read_buffer, NULL);
            geiger_data(read_value);
        }
    }
}

// stops reading message loop
void Geiger::stop_read_geiger() { read_mode = false; }
