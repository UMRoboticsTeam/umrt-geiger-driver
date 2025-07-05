//
//Created by Toni Odujinrin on 2025-07-05
//
/**
 * @file 
 * Class declaration for Geiger, provides functions for communicating with UMRTs serial geiger counter
 */ 


#ifndef GEIGER_HPP
#define GEIGER_HPP

#include <boost/signals2/signal.hpp>
#include <atomic>
#include <iostream>
#include <termios.h>



/**
* Manges serial communication between geiger counter and a Linux based SBC
*/
class Geiger{

public: 
    /**
     * Initializes serial geiger counter connection 
     * @param addr address for the serial geiger counter.  
     */
	Geiger(const std::string& addr);

	Geiger(const Geiger&) = delete; 

	Geiger& operator=(const Geiger&) = delete; 

	~Geiger(); 
    
    
	boost::signals2::signal<void(std::string)> geiger_data; 
    /**
     * begins a reading loop  
     * @param buff_size message buffer size.  
     */
	void read_geiger(size_t buff_size);
 
    /**
     * stops reading loop  
     */
	void stop_read_geiger();  

private: 
	int fd{-1}; 
	std::atomic<bool> read_mode =true; 
	struct termios termios_struct; 
}; 


#endif