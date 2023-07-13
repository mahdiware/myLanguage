#ifndef UTILS_H
#define UTILS_H



#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

#include <stdint.h>
#include <stdbool.h>

#if defined(_WIN32)
#include <windows.h>
typedef unsigned __int64    nanotime_t;
#define DIRREF              HANDLE
#else
#include <dirent.h>
typedef uint64_t            nanotime_t;
#define DIRREF              DIR*
#endif


nanotime_t nanotime (void);

double microtime (nanotime_t, nanotime_t);

double millitime (nanotime_t, nanotime_t);


#define prnt(size, ...) \
	char strprnt[size]; \
	sprintf(strprnt, __VA_ARGS__); \
	puts(strprnt); \


char* replace(const char*, const char*, const char*);

void ScapeSequence(char **);

// Function to convert an integer to a string
char* intToString(int);

// Function to convert a float to a string
char* floatToString(float number);

// Function to convert a string to an integer
int stringToInt(const char* str);

float stringToFloat(const char*);

//contains("Hello, World!", "World")
int contains(const char *str, const char *);

char** split(const char *, const char *, int *);
#endif