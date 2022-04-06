#include "log.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <ctime> 

Logger::Logger(std::string filename): logstream{} {
	logstream.open(filename, std::ios::app);
	if (!logstream) {
		throw std::runtime_error("Could not open file");
	}
	time_t now = time(nullptr) ;
	logstream << "\n" << std::put_time(localtime(&now), "%T") << " Log started " << std::endl;
}

Logger::~Logger() {
	logstream.close();
}

void Logger::log(std::string type, float value) {
	time_t now = time(nullptr) ;
	logstream << std::put_time(localtime(&now), "%T") << " " << type << ": " << value << std::endl;
}
