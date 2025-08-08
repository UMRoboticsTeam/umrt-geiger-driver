#include "geiger.hpp"
#include <boost/log/trivial.hpp>
#include <boost/signals2/signal.hpp>
#include <cerrno>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
using std::runtime_error;

Geiger::Geiger(const std::string& addr, unsigned int baud_rate) {
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
    termios_struct.c_cflag |= (CS8 | CREAD | CLOCAL);     // 8-bit data width, enable receiver, ignore modem controls

    // set baud rate based on input parameter
    speed_t baud;
    switch (baud_rate) {
        case 9600:
            baud = B9600;
            break;
        case 19200:
            baud = B19200;
            break;
        case 38400:
            baud = B38400;
            break;
        case 57600:
            baud = B57600;
            break;
        case 115200:
            baud = B115200;
            break;
        case 230400:
            baud = B230400;
            break;
        default:
            close(fd);
            fd = -1;
            BOOST_LOG_TRIVIAL(error) << "[x] unsupported baud rate: " << baud_rate;
            throw runtime_error("unsupported baud rate");
    }

    cfsetospeed(&termios_struct, baud);  // set output baud rate
    cfsetispeed(&termios_struct, baud);  // set input baud rate
    termios_struct.c_cc[VMIN] = 1;
    termios_struct.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &termios_struct) == -1) { // set configuration immediately
        close(fd);
        fd = -1;
        BOOST_LOG_TRIVIAL(error) << "[x] could not set termios config. errno: " << errno;
        throw runtime_error("could not get current termios config");
    }
    BOOST_LOG_TRIVIAL(info) << "[o] Termios custom config set with baud rate: " << baud_rate;
}

Geiger::~Geiger() {
    stop_read_geiger(); 
    if (fd >= 0) { close(fd); }
    fd = -1;
}

// read loop for reading geiger counter messages
void Geiger::read_geiger(size_t buff_size) {
    if (fd == -1) {
        BOOST_LOG_TRIVIAL(error) << "[x] serial file descriptor not valid";
        throw runtime_error("serial file descriptor not valid");
    }

    BOOST_LOG_TRIVIAL(debug) << "[?] starting read";
    read_mode = 1;

    std::vector<char> read_buffer(buff_size + 1); 

    while (read_mode) {
        ssize_t nbytes = read(fd, read_buffer.data(), buff_size);

        if (nbytes == -1) {
            BOOST_LOG_TRIVIAL(error) << "[x] failed to read from device";
            continue;
        }

        read_buffer[nbytes] = '\0'; 

        std::string payload(read_buffer.data(), nbytes);
        BOOST_LOG_TRIVIAL(debug) << "[?] payload: " << payload;

        char* end_ptr;
        double read_value = strtod(read_buffer.data(), &end_ptr);
        if (end_ptr == read_buffer.data()) {
            BOOST_LOG_TRIVIAL(warning) << "[!] Could not parse number from: " << payload;
            continue;
        }
        geiger_data(read_value);  
    }
}

// stops reading message loop
void Geiger::stop_read_geiger() { read_mode = false; }