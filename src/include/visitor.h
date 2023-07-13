#pragma once

#ifndef VISITOR_H
#define VISITOR_H
#include <stdbool.h>

static struct {
    size_t size;                                 // Current number of variables stored in the varstore

    struct {
        const uint8_t *beg;                       // Beginning memory address of the variable name
        ptrdiff_t len;                            // Length of the variable name
        size_t array_size;                        // Size of the array, if the variable is an array
        int *values;                              // Pointer to the array of variable values
        bool boolean;							// bool variable
        uint8_t type;							 // Type of VarStore
        const uint8_t *string;                   // Beginning memory address of the String Value
        float Float;
    } vars[VAR_CAPACITY];                // Array to store the variables
} varstore;

static struct {
	size_t size;                                 // Current number of functions stored in the function

	struct {
		const uint8_t *beg;                       // Beginning memory address of the function name
        ptrdiff_t len;
        struct node *children;
	} get[VAR_CAPACITY];										//

} function;

struct node;

void visitor(const struct node *);

#endif