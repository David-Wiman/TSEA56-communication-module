#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

enum severity{INFO, DEBUG, WARNING, ERROR};

class Logger {
public:
	Logger(std::string filename);
	~Logger();
	
	void log(int serverity, std::string origin, std::string type, float value);
	void log(int serverity, std::string origin, std::string type, std::string value);

private:
	std::fstream logstream;

};


#endif  // LOGGER_H
