#include "Log.h"

Log::Log(const char* logName) {
	Create(logName);
}

Log::~Log(void) {
	Close();
}

void Log::Create(const char* logName) {
	file.open(logName, std::ofstream::out | std::ofstream::trunc);
	initialized = file.is_open();

	if (!initialized) {
		std::printf("Cannot create log file: %s", logName);
	}
}

void Log::Write(std::string msg) {
	assert(initialized == true);
	file << msg << std::endl;
}

void Log::Write(int lvl, std::string msg) {
	assert(initialized == true);
	std::string level;
	switch (lvl) {
	case 0:
		level = "[DEBUG]:";
		break;
	case 1:
		level = "[INFO]:";
		break;
	case 2:
		level = "[WARN]:";
		break;
	case 3:
		level = "[ERROR]:";
		break;
	default:
		level = "[INFO]:";
	}
	file << level << msg << std::endl;
}

void Log::Printf(const char* format, ...) {
	assert(initialized == true);
	char msg[4096] = { };

	va_list args;
	va_start(args, format);

	if (vsnprintf(msg, sizeof(msg), format, args) != -1) {
		file << msg << std::endl;
	}

	va_end(args);

}

void Log::Printf(int lvl, const char* format, ...) {
	assert(initialized == true);
	char msg[4096] = { };

	va_list args;
	va_start(args, format);

	if (vsnprintf(msg, sizeof(msg), format, args) != -1) {
		Write(lvl, msg);
	}

	va_end(args);
}

void Log::Close() {
	if (initialized) {
		file.close();
	}
}

