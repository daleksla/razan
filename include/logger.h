#ifndef LOGGER_H
#define LOGGER_H
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief File stores functionality for logging data
 * @author Salih MSA
 */


#define ANSI_REGULAR_TEXT   "\033[22m"
#define ANSI_BOLD_TEXT      "\033[1m"
#define ANSI_COLOUR_WHITE   "\033[97m"
#define ANSI_COLOUR_YELLOW  "\033[93m"
#define ANSI_COLOUR_ORANGE  "\033[33m"
#define ANSI_COLOUR_RED     "\033[31m"
#define ANSI_COLOUR_RESET   "\033[0m"


enum printl_type {
	INFO = 0, /* for reporting the successful occurance of a standard yet required event. Typically, these will be used to report whether a feature has executed successfully */
	DEBUG, /* for reporting an occurance, noteworthy or not, for dev purposes. Typically, these are for reporting incremental steps in a function which in production would only merit a single status report on whether it failed or succeeded */
	WARN, /* for reporting a foreseeable but nonetheless counterproductive issue. Typically, it means an activity will fail but the server won't */
	REPORT /* these are for issues, most likely fatal errors. Typically, this means the death of the server */
} ;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief printl - wrapper around printf function designed to log annotated, uninterrupted messages
 * @param const enum printl_type printl - type of print loging priority
 * This will determine not only formatting, but also which channel messages go to:
 	* INFO, DEBUG messages go to stdout
 	* WARN, REPORT go to stderr
 * @param const char *msg - actual message to be printed with same % formatting tokens as printf family
 */
void printl(const enum printl_type printl, const char *msg, ...) ;

#ifdef __cplusplus
}
#endif

#endif /* LOGGER_H */
