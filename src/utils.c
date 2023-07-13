#include "include/utils.h"
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

#include <stddef.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


#if defined(__linux)
#include <sys/time.h>
#endif
#if defined(__MACH__)
#include <mach/mach_time.h>
#endif
#if defined(_WIN32)
#include <windows.h>
#include <Shlwapi.h>
#include <tchar.h>
#include <io.h>
#endif
#if defined(__EMSCRIPTEN__)
#include <sys/time.h>
#endif


nanotime_t nanotime (void) {
    nanotime_t value;
    
    #if defined(_WIN32)
    static LARGE_INTEGER    win_frequency;
    QueryPerformanceFrequency(&win_frequency);
    LARGE_INTEGER            t;
    
    if (!QueryPerformanceCounter(&t)) return 0;
    value = (t.QuadPart / win_frequency.QuadPart) * 1000000000;
    value += (t.QuadPart % win_frequency.QuadPart) * 1000000000 / win_frequency.QuadPart;
    
    #elif defined(__MACH__)
    mach_timebase_info_data_t    info;
    kern_return_t                r;
    nanotime_t                    t;
    
    t = mach_absolute_time();
    r = mach_timebase_info(&info);
    if (r != 0) return 0;
    value = (t / info.denom) * info.numer;
    value += (t % info.denom) * info.numer / info.denom;
    
    #elif defined(__linux)
    struct timespec ts;
    int                r;
    
    r = clock_gettime(CLOCK_MONOTONIC, &ts);
    if (r != 0) return 0;
    value = ts.tv_sec * (nanotime_t)1000000000 + ts.tv_nsec;
    
    #else
    struct timeval    tv;
    int                r;
    
    r = gettimeofday(&tv, 0);
    if (r != 0) return 0;
    value = tv.tv_sec * (nanotime_t)1000000000 + tv.tv_usec * 1000;
    #endif
    
    return value;
}

double microtime (nanotime_t tstart, nanotime_t tend) {
    nanotime_t t = tend - tstart;
    return ((double)t / 1000.0f);
}

double millitime (nanotime_t tstart, nanotime_t tend) {
    nanotime_t t = tend - tstart;
    return ((double)t / 1000000.0f);
}


/***_OTHER_***/

uint8_t *file_reader(const char *path, size_t *size, int *exit_status){
	int fd;
	struct stat statbuf;
	
	*exit_status = EXIT_FAILURE;
	if ((fd = open(path, O_RDONLY)) < 0) {
        perror("open"), *exit_status;
        return NULL;
    }
    
    if (fstat(fd, &statbuf) < 0) {
        perror("fstat"), close(fd), *exit_status;
        return NULL;
    }
    
    if ((*size = statbuf.st_size) == 0) {
        fprintf(stderr, "‘%s‘: file is empty\n", path);
        close(fd), *exit_status;
        return NULL;
    }
    
    uint8_t *source_code = mmap(0, *size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (source_code == MAP_FAILED) {
        perror("mmap"), close(fd), *exit_status;
        return NULL;
    }
    *exit_status = EXIT_SUCCESS;
    
    close(fd);
    return source_code;
}


char* replace(const char* str, const char* find, const char* replace) {
    char* result;
    int i, count = 0;
    size_t find_len = strlen(find);
    size_t replace_len = strlen(replace);
    // Count the number of occurrences of 'find' in 'str'
    for (i = 0; str[i] != '\0'; i++) {
        if (strstr(&str[i], find) == &str[i]) {
            count++;
            i += find_len - 1;
        }
    }
    // Allocate memory for the result string
    result = (char*)malloc(i + count * (replace_len - find_len) + 1);
    i = 0;
    // Replace occurrences of 'find' with 'replace'
    while (*str) {
        if (strstr(str, find) == str) {
            strcpy(&result[i], replace);
            i += replace_len;
            str += find_len;
        } else {
            result[i++] = *str++;
        }
    }
    result[i] = '\0';
    return result;
}

void ScapeSequence(char **stringPtr){
	*stringPtr = replace(*stringPtr, "\\n", "\n");
	*stringPtr = replace(*stringPtr, "\\a", "\a");
	*stringPtr = replace(*stringPtr, "\\b", "\b");
	*stringPtr = replace(*stringPtr, "\\f", "\f");
	*stringPtr = replace(*stringPtr, "\\r", "\r");
	*stringPtr = replace(*stringPtr, "\\t", "\t");
	*stringPtr = replace(*stringPtr, "\\v", "\v");
}

// Function to convert an integer to a string
char* intToString(int number) {
    int length = snprintf(NULL, 0, "%d", number);
    char* str = (char*)malloc(length + 1);
    snprintf(str, length + 1, "%d", number);
    return str;
}

// Function to convert a float to a string
char* floatToString(float number) {
    int length = snprintf(NULL, 0, "%f", number);
    char* str = (char*)malloc(length + 1);
    snprintf(str, length + 1, "%f", number);
    return str;
}

// Function to convert a string to an integer
int stringToInt(const char* str) {
    return atoi(str);
}

float stringToFloat(const char* str) {
    float result = 0.0f;
    float fraction = 0.0f;
    int numDigits = 0;
    int fractionDigits = 0;
    
    // Convert the integer part
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
        numDigits++;
    }
    // Convert the fraction part
    if (*str == '.') {
        str++; // Skip the decimal point
        while (*str >= '0' && *str <= '9') {
            fraction = fraction * 10 + (*str - '0');
            str++;
            fractionDigits++;
        }
    }
    // Calculate the divisor to convert fraction to float
    float divisor = 1.0f;
    for (int i = 0; i < fractionDigits; i++) {
        divisor *= 10.0f;
    }
    // Combine the integer and fraction parts
    result += fraction / divisor;
    
    return result;
}

//contains("Hello, World!", "World")
int contains(const char *str, const char *substring) {
    if (strstr(str, substring) != NULL) {
        return 1;  // Substring found
    } else {
        return 0;  // Substring not found
    }
}

/*Example:
	const char str[] = "Hello,World,Example";
    const char delimiter[] = ",";
    
    int resultLength = 0;
    char **result = split(str, delimiter, &resultLength);

    printf("Split substrings:\n");
    for (int i = 0; i < resultLength; i++) {
        printf("%s\n", result[i]);
        free(result[i]);  
    }
    free(result);
*/
char** split(const char *str, const char *delimiter, int *resultLength) {
    int i = 0;
    char *copy = strdup(str);  // Create a copy of the input string
    char *token = strtok(copy, delimiter);

    // Count the number of substrings
    while (token != NULL) {
        i++;
        token = strtok(NULL, delimiter);
    }

    // Allocate memory for the result array
    char **result = (char **)malloc(i * sizeof(char *));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    // Reset the copy and split again to fill the result array
    strcpy(copy, str);
    i = 0;
    token = strtok(copy, delimiter);

    // Store each substring in the result array
    while (token != NULL) {
        result[i] = strdup(token);
        i++;
        token = strtok(NULL, delimiter);
    }

    *resultLength = i;
    free(copy);  // Free the memory allocated for the copy

    return result;
}