#include "log.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <ctime> 

/* Create and open log file, log time */
Logger::Logger(std::string filename): logstream{} {
	logstream.open(filename, std::ios::app);
	if (!logstream) {
		throw std::runtime_error("Could not open file");
	}
	time_t now = time(nullptr) ;
	logstream << "\n" << std::put_time(localtime(&now), "%T") << " Log started " << std::endl;
}

/* Close the log file upon destruction */
Logger::~Logger() {
	logstream.close();
}

/*  Write to log file */
void Logger::log(int severity, std::string origin, std::string type, float value) {
	log(severity, origin, type, std::to_string(value));
}

void Logger::log(int severity, std::string origin, std::string type, std::string value) {
	time_t now = time(nullptr) ;
	logstream << std::put_time(localtime(&now), "%T") << " " << severity << ", " << origin << ", " << type << ": " << value << std::endl;
}


