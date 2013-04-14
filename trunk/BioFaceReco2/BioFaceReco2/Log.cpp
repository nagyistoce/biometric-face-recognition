#include "Log.h"


Log::Log(const char* logName)
{
	Create(logName);
}

Log::~Log(void)
{
	Close();
	delete stream;
	delete file;
}

void Log::Create(const char* logName)
{
	file = new QFile(QString(logName));
	initialized = file->open(QIODevice::WriteOnly | QIODevice::Text);

	if(initialized) {
		stream = new QTextStream(file);
	}
}

void Log::Write(std::string msg)
{
	if(initialized)
	{
		*stream << msg.c_str();
	}
}
	
void Log::Write(const char* msg)
{
	if(initialized)
	{
		*stream << msg;
	}
}

void Log::Printf(const char* format, ...)
{
	if(initialized)
	{
		char msg[4096] = {};

		va_list args;
		va_start(args, format);

		if( _vsnprintf_s(msg, sizeof(msg), format, args) != -1)
		{
			*stream << msg;
		}

		va_end(args);
	}
}

void Log::Close()
{
	if(initialized)
	{
		file->close();
	}
}


	