#include "Log.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

#define CONSOLE_DEFAULT_COLOR 15

HANDLE Logger::hStdout{};
bool Logger::logToConsole{};
bool Logger::logToFile{};
std::ofstream Logger::logFile{};
bool Logger::logDate{ false };
bool Logger::logTime{ true };

Logger::~Logger()
{
	if (logFile.is_open()) {
		logFile.close();
	}
}

Logger& Logger::Get() {
	static Logger instance;
	return instance;
}

void Logger::Init(bool LogToConsole/*  = true  */, bool LogToFile/*  = false  */, const std::string& filepath/*  = LogFile  */) {
	
	Logger::logToConsole = LogToConsole;
	Logger::logToFile = LogToFile;

	if (logToConsole) {
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hStdout == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, TEXT("Invalid StdOut handle, no console logging"), TEXT("Logger::Init"), NULL);
		}
	}
	if (LogToFile) {
		logFile.open(filepath);
	}
}

void Logger::baselog(const std::string& msg, int color) {
	if (!logToConsole) return;
	if (logTime) { std::cout << TimeStamp() << " "; }
	SetConsoleTextAttribute(hStdout, color);
	std::cout << msg << std::endl;
	SetConsoleTextAttribute(hStdout, CONSOLE_DEFAULT_COLOR);


	if (!logToFile) return;


}

void Logger::info(std::string msg) {
	baselog(msg, 3);

}
void Logger::warning(std::string msg) {
	baselog(msg, 14);
}
void Logger::error(std::string msg) {
	baselog(msg, 12);
}


std::string Logger::TimeStamp() {
	std::time_t time{ std::time(NULL) };
	std::tm now{};
	localtime_s(&now, &time);

	std::ostringstream oss;

	if (logTime) {
		oss << "["
			<< std::setfill('0') << std::setw(2) << now.tm_hour
			<< ":"
			<< std::setfill('0') << std::setw(2) << now.tm_min
			<< ":"
			<< std::setfill('0') << std::setw(2) << now.tm_sec
			<< "]";
	}

	return oss.str();
}