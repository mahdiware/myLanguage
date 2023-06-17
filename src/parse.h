#pragma once

#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>
#include <stddef.h>

//if development is 1 then it is defined and displayed to the lexer and interpreter when running the code.
#ifndef development
	#define development 1
#endif
//capacity of variable & char length
#ifndef VAR_CAPACITY
	#define VAR_CAPACITY 128
#endif

enum {
    NT_Unit,   // Non-terminal symbol for the entire program or unit
    NT_Stmt,   // Non-terminal symbol for a statement
    //NT_Iden, 	//
    NT_Assn,   // Non-terminal symbol for an assignment statement
    NT_Prnt,   // Non-terminal symbol for a print statement
    NT_Inpt,   // Non-terminal symbol for an input statement
    NT_Ctrl,   // Non-terminal symbol for a control statement (e.g., if, while, do-while)
    //NT_Func,	// Non-terminal symbol for an function statement
    NT_Cond,   // Non-terminal symbol for a conditional expression (e.g., if condition)
    NT_Elif,   // Non-terminal symbol for an else-if clause
    NT_Else,   // Non-terminal symbol for an else clause
    NT_Dowh,   // Non-terminal symbol for a do-while loop
    NT_Whil,   // Non-terminal symbol for a while loop
    NT_Atom,   // Non-terminal symbol for an atomic expression
    NT_Expr,   // Non-terminal symbol for an expression
    NT_Pexp,   // Non-terminal symbol for a primary expression
    NT_Bexp,   // Non-terminal symbol for a boolean expression
    NT_Uexp,   // Non-terminal symbol for a unary expression
    NT_Texp,   // Non-terminal symbol for a term expression
    NT_Aexp,   // Non-terminal symbol for an arithmetic expression
    NT_COUNT   // Total count of non-terminal symbols
};

struct token;
struct node {
    /* use "token" if nchildren == 0, "nt" and "children" otherwise */
    uint32_t nchildren;

    union {
        const struct token *token;

        struct {
            uint8_t nt;
            struct node **children;
        };
    };
};

struct node parse(const struct token *, size_t);

enum {
    PARSE_OK,     // Parsing was successful
    PARSE_REJECT, // Parsing was rejected or encountered an error
    PARSE_NOMEM,  // Parsing failed due to memory allocation failure
};

#define parse_error(root) ({ \
    struct node root_once = (root); \
    root_once.nchildren ? PARSE_OK : root_once.token->tk; \
})

void destroy_tree(struct node);

#endif