#pragma once

#include <string>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

//Logger
//Class to crate and write file logs.
class Log {
public:
	Log(const char * logName);
	~Log(void);

	void Write(std::string msg);
	void Write(int lvl, std::string msg);
	void Printf(const char* format, ...);
	void Printf(int lvl, const char* format, ...);

private:
	std::ofstream file;
	bool initialized;

	void Create(const char* logName);
	void Close();
};

