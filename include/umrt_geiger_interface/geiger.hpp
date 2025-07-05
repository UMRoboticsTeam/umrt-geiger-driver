#ifndef GEIGER_HPP
#define GEIGER_HPP

#include <atomic>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <cerrno>
#include <boost/signals2/signal.hpp>
#include <boost/log/trivial.hpp>


class Geiger{

public: 
	Geiger(const std::string& addr);
	Geiger(const Geiger&) = delete; 
	Geiger& operator=(const Geiger&) = delete; 

	~Geiger(); 
	boost::signals2::signal<void(std::string)> geiger_data; 
	void read_geiger(size_t buff_size); 
	void stop_read_geiger();  

private: 
	int fd{-1}; 
	std::atomic<bool> read_mode =true; 
	struct termios termios_struct; 
}; 


#endif