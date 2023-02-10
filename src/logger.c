#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "logger.h"

/**
 * @brief File stores implementations of functionality to printl
 * @author Salih MSA
 */

void printl(const enum printl_type printl, const char *msg, ...)
{
#ifndef DEV_MODE
	if (printl == DEBUG) /* if we're not in developement mode, do not print any debug statements */
		return;
#endif /* DEV_MODE */

	static const char *log_formats[] = {ANSI_COLOUR_WHITE, ANSI_COLOUR_YELLOW, ANSI_COLOUR_ORANGE, ANSI_COLOUR_RED};

	const char *log_format = log_formats[printl];
	time_t tmi;
	time(&tmi);
	struct tm *utc_time;
	utc_time = gmtime(&tmi);
	va_list args;
	va_start(args, msg); /* start list of args from msg */
	FILE *const fhand = (printl == INFO || printl == DEBUG) ? stdout : stderr ; /* const ptr, reg data */

	flockfile(stdout);
	flockfile(stderr);
	fprintf(fhand, "%s%s", log_format, ANSI_BOLD_TEXT);
	fprintf(fhand, "[%2d:%02d:%02d UTC]: ", (utc_time->tm_hour) % 24, utc_time->tm_min, utc_time->tm_sec);
	fprintf(fhand, "%s%s", ANSI_REGULAR_TEXT, log_format);
	vfprintf(fhand, msg, args);
	fprintf(fhand, "%s\n", ANSI_COLOUR_RESET); /* at this point, clear colour and buffer flushes */
	funlockfile(stdout);
	funlockfile(stderr);
}
