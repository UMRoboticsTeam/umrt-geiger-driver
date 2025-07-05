#include "../umrt_geiger_interface/geiger.hpp"
#include <thread>
#include <iostream>



int main(size_t argc, char*argv[]){
	std::string addr = "/dev/ttyACM0";
	int buff_size = 1000;  
	if(argc > 1 && sizeof(argv[1]) > 0){
		if(sizeof(argv[1] > 0)){
			addr = argv[1]; 
		}
		if(argc > 2){
			buff_size = strtol(argv[2], NULL, 10); 
		}
	}

	Geiger geiger_instance(addr);
	std::thread read_thread([&](){
		geiger_instance.read_geiger(buff_size); 
	});

	geiger_instance.geiger_data.connect([](std::string msg){std::cout<<msg<<std::endl;});



}