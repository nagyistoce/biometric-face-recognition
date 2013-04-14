#pragma once

#include <qfile.h>
#include <qtextstream.h>

//Logger
//Class to crate and write file logs.
class Log
{
public:
	Log(const char* logName);
	~Log(void);
	
	void Write(std::string msg);
	void Write(const char* msg);
	void Printf(const char* format, ...);

private:
	QFile * file;
	QTextStream * stream;
	bool initialized;

	void Create(const char* logName);
	void Close();
};

