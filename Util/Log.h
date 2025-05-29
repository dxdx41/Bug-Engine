#pragma once
#include <Windows.h>
#include <string>
#include <fstream>

class Logger {
public:
	static Logger& Get();

	static void Init(bool LogToConsole = true, bool LogToFile = false, const std::string& filepath = "LogFile");
	
	void info(std::string msg);
	void warning(std::string msg);
	void error(std::string msg);
private:
	static HANDLE hStdout;
	static bool logToConsole;
	static bool logToFile;
	static std::ofstream logFile;
	static bool logDate;
	static bool logTime;

	Logger() = default;
	~Logger();

	void baselog(const std::string& msg, int color);
	std::string TimeStamp();

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
};

#define Log Logger::Get()