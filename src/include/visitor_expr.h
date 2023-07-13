#pragma once

#ifndef VISITOR_EXPR_H
#define VISITOR_EXPR_H

#include "lexer.h"
#include "parser.h"
#include "visitor.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


static void eval_atom(const struct node *const, uint8_t *, int *, float *, bool *, char **);     // Function to evaluate an atomic expression node
static void eval_expr(const struct node *const, uint8_t *, int *, float *, bool *, char **);     // Function to evaluate a generic expression node
static void eval_cexp(const struct node *const, uint8_t *, int *, float *, bool *, char **);
static void eval_pexp(const struct node *const, uint8_t *, int *, float *, bool *, char **);     // Function to evaluate a parenthesized expression node
static void eval_cexp(const struct node *const, uint8_t *, int *, float *, bool *, char **);
static void eval_bexp(const struct node *const, uint8_t *, int *, float *, bool *, char **);     // Function to evaluate a boolean expression node
static void eval_uexp(const struct node *const, uint8_t *, int *, float *, bool *, char **);     // Function to evaluate a unary expression node
static void eval_texp(const struct node *const, uint8_t *, int *, float *, bool *, char **);     // Function to evaluate a ternary expression node
static void eval_aexp(const struct node *const, uint8_t *, int *, float *, bool *, char **);     // Function to evaluate an arithmetic expression node

enum {
	TYPE_INT,			//Variable type integer
	TYPE_STR,			//Variable type string
	TYPE_FLT,			//Variable type float
	TYPE_BOL,			//Variable type bool
};

static uint8_t get_type_expr(const struct node *const expr_t)
{
	int g_int = 0;
	float g_flt = 0.0;
	bool g_bol = false;
	char *g_str = NULL;
	uint8_t type;
	eval_expr(expr_t, &type, &g_int, &g_flt, &g_bol, &g_str);
	return type;
}

static int get_int_expr(const struct node *const expr_t)
{
	int g_int = 0;
	float g_flt = 0.0;
	bool g_bol = false;
	char *g_str = NULL;
	uint8_t type;
	eval_expr(expr_t, &type, &g_int, &g_flt, &g_bol, &g_str);
	return g_int;
}

static float get_flt_expr(const struct node *const expr_t)
{
	int g_int = 0;
	float g_flt = 0.0;
	bool g_bol = false;
	char *g_str = NULL;
	uint8_t type;
	eval_expr(expr_t, &type, &g_int, &g_flt, &g_bol, &g_str);
	return g_flt;
}

static char *get_str_expr(const struct node *const expr_t)
{
	int g_int = 0;
	float g_flt = 0.0;
	bool g_bol = false;
	char *g_str = NULL;
	uint8_t type;
	eval_expr(expr_t, &type, &g_int, &g_flt, &g_bol, &g_str);
	return g_str;
}

static bool get_bol_expr(const struct node *const expr_t)
{
	int g_int = 0;
	float g_flt = 0.0;
	bool g_bol = false;
	char *g_str = NULL;
	uint8_t type;
	eval_expr(expr_t, &type, &g_int, &g_flt, &g_bol, &g_str);
	return g_bol;
}

static void eval_atom(const struct node *const atom, uint8_t *type, int *intPtr, float *floatPtr, bool *boolPtr, char **stringPtr)
{
    switch (atom->children[0]->token->tk) {
        case TK_NAME: {
            // Variable name
            const uint8_t *const beg = atom->children[0]->token->beg;
            const ptrdiff_t len = atom->children[0]->token->end - beg;

            // Search for the variable in the varstore
            for (size_t idx = 0; idx < varstore.size; ++idx) {
                if (varstore.vars[idx].len == len && !memcmp(varstore.vars[idx].beg, beg, len)) {
                    // Check if the variable is an array
                    if (varstore.vars[idx].array_size) {
                        
                        if(varstore.vars[idx].type == TYPE_STR){
                        	*stringPtr = (char *)varstore.vars[idx].string;
        					*type = TYPE_STR;
                        }else if(varstore.vars[idx].type == TYPE_INT){
                        	*intPtr = varstore.vars[idx].values[0];
                        	*type = TYPE_INT;
                        }else if(varstore.vars[idx].type == TYPE_FLT){
                        	*floatPtr = varstore.vars[idx].Float;
                        	*type = TYPE_FLT;
                        }
                        
                        return;
                    } else {
                        *intPtr = 0;
                        return;
                    }
                }
            }
            
            fprintf(stderr, "warn: access to undefined variable\n");
            *intPtr = 0;
            return;
        }
        case TK_STRL: {
        	const uint8_t *const beg = atom->children[0]->token->beg + 1;
        	const ptrdiff_t len = (atom->children[0]->token->end - 1) - beg;
        	
        	char *tempstring = (char *)malloc((len+1) * sizeof(char *));
	        sprintf(tempstring, "%.*s", len, beg);
        	
        	ScapeSequence(&tempstring);
        	
        	*stringPtr = tempstring;
        	*type = TYPE_STR;
        	return;
        }
        case TK_NMBR: {
            // Numeric value
            const uint8_t *const beg = atom->children[0]->token->beg;
            const uint8_t *const end = atom->children[0]->token->end;

            char *StrNum = (char *)malloc((end - beg + 1) * sizeof(char));
            sprintf(StrNum, "%.*s", end - beg, beg);

            if (contains(StrNum, ".")) {
                *floatPtr = stringToFloat(StrNum);
                *type = TYPE_FLT;
            } else {
                *intPtr = stringToInt(StrNum);
                *type = TYPE_INT;
            }

            return;
        }

        default: {
            abort();
        }
    }
}

static void eval_expr(const struct node *const expr, uint8_t *type, int *intPtr, float *floatPtr, bool *boolPtr, char **stringPtr)
{
    switch (expr->children[0]->nt) {
        case NT_Atom:
            eval_atom(expr->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            break;

		case NT_Cexp:
            eval_cexp(expr->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            break;

        case NT_Pexp:
            eval_pexp(expr->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            break;

        case NT_Bexp:
            eval_bexp(expr->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            break;

        case NT_Uexp:
            eval_uexp(expr->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            break;

        case NT_Texp:
            eval_texp(expr->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            break;

        case NT_Aexp:
            eval_aexp(expr->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            break;

        default:
            abort();
    }
}

static void eval_cexp(const struct node *const cexp, uint8_t *type, int *intPtr, float *floatPtr, bool *boolPtr, char **stringPtr)
{
	eval_expr(cexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
}

static void eval_pexp(const struct node *const pexp, uint8_t *type, int *intPtr, float *floatPtr, bool *boolPtr, char **stringPtr)
{
    // Parenthesized expression, evaluate the expression inside
    eval_expr(pexp->children[1], type, intPtr, floatPtr, boolPtr, stringPtr);
}

static void eval_bexp(const struct node *const bexp, uint8_t *type, int *intPtr, float *floatPtr, bool *boolPtr, char **stringPtr)
{
    switch (bexp->children[1]->token->tk) {
        case TK_PLUS:
            // Addition
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            int tempInt1 = *intPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            int tempInt2 = *intPtr;
            *intPtr = tempInt1 + tempInt2;
            break;

        case TK_MINS:
            // Subtraction
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt1 = *intPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt2 = *intPtr;
            *intPtr = tempInt1 - tempInt2;
            break;

        case TK_MULT:
            // Multiplication
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt1 = *intPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt2 = *intPtr;
            *intPtr = tempInt1 * tempInt2;
            break;

        case TK_DIVI: {
            // Division
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt1 = *intPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt2 = *intPtr;

            if (tempInt2) {
                *intPtr = tempInt1 / tempInt2;
            } else {
                fprintf(stderr, "warn: prevented attempt to divide by zero\n");
                *intPtr = 0;
            }
            break;
        }

        case TK_MODU:
            // Modulo
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt1 = *intPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt2 = *intPtr;
            *intPtr = tempInt1 % tempInt2;
            break;

        case TK_EQUL:
            // Equality
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt1 = *intPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt2 = *intPtr;
            *boolPtr = tempInt1 == tempInt2;
            *intPtr = *boolPtr;
            break;

        case TK_NEQL:
            // Inequality
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt1 = *intPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt2 = *intPtr;
            *boolPtr = tempInt1 != tempInt2;
            *intPtr = *boolPtr;
            break;

        case TK_LTHN:
            // Less than
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt1 = *intPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt2 = *intPtr;
            *boolPtr = tempInt1 < tempInt2;
            *intPtr = *boolPtr;
            break;

        case TK_GTHN:
            // Greater than
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt1 = *intPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt2 = *intPtr;
            *boolPtr = tempInt1 > tempInt2;
            *intPtr = *boolPtr;
            break;

        case TK_LTEQ:
            // Less than or equal to
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt1 = *intPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt2 = *intPtr;
            *boolPtr = tempInt1 <= tempInt2;
            *intPtr = *boolPtr;
            break;

        case TK_GTEQ:
            // Greater than or equal to
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt1 = *intPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempInt2 = *intPtr;
            *boolPtr = tempInt1 >= tempInt2;
            *intPtr = *boolPtr;
            break;

        case TK_CONJ:
            // Logical conjunction (AND)
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            bool tempBool1 = *boolPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            bool tempBool2 = *boolPtr;
            *boolPtr = tempBool1 && tempBool2;
            *intPtr = *boolPtr;
            break;

        case TK_DISJ:
            // Logical disjunction (OR)
            eval_expr(bexp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempBool1 = *boolPtr;
            eval_expr(bexp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
            tempBool2 = *boolPtr;
            *boolPtr = tempBool1 || tempBool2;
            *intPtr = *boolPtr;
            break;

        default:
            abort();
    }
}

static void eval_uexp(const struct node *const uexp, uint8_t *type, int *intPtr, float *floatPtr, bool *boolPtr, char **stringPtr)
{
    // Evaluate unary expressions
    switch (uexp->children[0]->token->tk) {
        case TK_PLUS:
            // Unary plus
            eval_expr(uexp->children[1], type, intPtr, floatPtr, boolPtr, stringPtr);
            break;

        case TK_MINS:
            // Unary minus
            eval_expr(uexp->children[1], type, intPtr, floatPtr, boolPtr, stringPtr);
            *intPtr = -*intPtr;
            break;

        case TK_NEGA:
            // Logical negation
            eval_expr(uexp->children[1], type, intPtr, floatPtr, boolPtr, stringPtr);
            *boolPtr = !*boolPtr;
            break;

        default:
            abort();
    }
}

static void eval_texp(const struct node *const texp, uint8_t *type, int *intPtr, float *floatPtr, bool *boolPtr, char **stringPtr)
{
    // Evaluate ternary expressions
    eval_expr(texp->children[0], type, intPtr, floatPtr, boolPtr, stringPtr);
    bool condition = *boolPtr;
    if (condition) {
        eval_expr(texp->children[2], type, intPtr, floatPtr, boolPtr, stringPtr);
    } else {
        eval_expr(texp->children[4], type, intPtr, floatPtr, boolPtr, stringPtr);
    }
}

static void eval_aexp(const struct node *const aexp, uint8_t *type, int *intPtr, float *floatPtr, bool *boolPtr, char **stringPtr)
{
    // Evaluate array expressions
    const uint8_t *const beg = aexp->children[0]->token->beg;
    const ptrdiff_t len = aexp->children[0]->token->end - beg;
    int temp_idx = *intPtr;
    eval_expr(aexp->children[2], type, &temp_idx, floatPtr, boolPtr, stringPtr);
    const int array_idx = temp_idx;

    // Check if the array index is negative
    if (array_idx < 0) {
        fprintf(stderr, "warn: negative array offset\n");
        *intPtr = 0;
        return;
    }

    // Search for the array in the varstore
    for (size_t idx = 0; idx < varstore.size; ++idx) {
        if (varstore.vars[idx].len == len && !memcmp(varstore.vars[idx].beg, beg, len)) {
            // Check if the array index is within the bounds
            if (array_idx < varstore.vars[idx].array_size) {
                *intPtr = varstore.vars[idx].values[array_idx];
                return;
            } else {
                fprintf(stderr, "warn: out of bounds array access\n");
                *intPtr = 0;
                return;
            }
        }
    }

    fprintf(stderr, "warn: access to undefined array\n");
    *intPtr = 0;
}

#endif