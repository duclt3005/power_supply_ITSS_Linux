#include "utils.h"

int tprintf(const char *fmt, ...)
{
	va_list args;
	struct tm *tstruct;
	time_t tsec = time(NULL);
	tstruct = localtime(&tsec);
	printf("%02d:%02d:%02d: %5d| ", tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec, getpid());
	va_start(args, fmt);
	return vprintf(fmt, args);
}