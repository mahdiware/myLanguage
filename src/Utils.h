#ifndef UTILS
#define UTILS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define prnt(size, ...) \
	char strprnt[size]; \
	sprintf(strprnt, __VA_ARGS__); \
	puts(strprnt); \


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
#endif