#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

enum severity{INFO, DEBUG, WARNING, ERROR};
static const char * EnumStrings[] = { "INFO", "DEBUG", "WARNING", "ERROR" };

class Logger {
public:
	static void init();
	static void close();
	
	static void log(int serverity, std::string origin, std::string type, float value);
	static void log(int serverity, std::string origin, std::string type, std::string value);

private:
	Logger() {}
	static std::fstream logstream;

};


#endif  // LOGGER_H
