#include "log.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <ctime>

std::fstream Logger::logstream{};

/* Create and open log file, log time */
void Logger::init() {
	logstream.open("log/log.txt", std::ios::app);
	if (!logstream) {
		throw std::runtime_error("Could not open file");
	}
	time_t now = time(nullptr);
	logstream << "\n" << std::put_time(localtime(&now), "%T") << " Log started " << std::endl;
}

/* Close dsafthe log file upon destruction */
void Logger::close() {
	logstream.close();
}

/*  Write to log file */
void Logger::log(int severity, std::string origin, std::string type, float value) {
	log(severity, origin, type, std::to_string(value));
}

void Logger::log(int severity, std::string origin, std::string type, std::string value) {
	time_t now = time(nullptr) ;
	logstream << std::put_time(localtime(&now), "%T") << ", " << EnumStrings[severity] << ", " << origin << ", " << type << ": " << value << std::endl;
}
