#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

class Logger {
public:
	Logger(std::string filename);
	~Logger();
	
	void log(std::string type, float value);

private:
	std::fstream logstream;

};


#endif  // LOGGER_H
