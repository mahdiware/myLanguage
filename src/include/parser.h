#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stddef.h>

//capacity of variable & char length
#ifndef VAR_CAPACITY
	#define VAR_CAPACITY 1024
#endif

enum {
  /*00*/ NT_Unit,   // Non-terminal symbol for the entire program or unit
          NT_Stmt,   // Non-terminal symbol for a statement
          NT_Assn,   // Non-terminal symbol for an assignment statement
          NT_Ctrl,   // Non-terminal symbol for a control statement (e.g., if, while, do-while)
          NT_Func,	// Non-terminal symbol for an function statement
  /*05*/ NT_Cond,   // Non-terminal symbol for a conditional expression (e.g., if condition)
          NT_Elif,   // Non-terminal symbol for an else-if clause
          NT_Else,   // Non-terminal symbol for an else clause
          NT_Dowh,   // Non-terminal symbol for a do-while loop
          NT_Whil,   // Non-terminal symbol for a while loop
  /*10*/ NT_Atom,   // Non-terminal symbol for an atomic expression
          NT_Expr,   // Non-terminal symbol for an expression
          NT_Cexp,
          NT_Pexp,   // Non-terminal symbol for a Parenthesize expression
          NT_Bexp,   // Non-terminal symbol for a boolean expression
  /*15*/ NT_Uexp,   // Non-terminal symbol for a unary expression
          NT_Texp,   // Non-terminal symbol for a term expression
          NT_Aexp,   // Non-terminal symbol for an arithmetic expression
  /*18*/ NT_COUNT,   // Total count of non-terminal symbols
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

struct node parser(const struct token *, size_t);

enum {
    PARSER_OK,     // Parsing was successful
    PARSER_REJECT, // Parsing was rejected or encountered an error
    PARSER_NOMEM,  // Parsing failed due to memory allocation failure
};

#define parser_error(root) ({ \
    struct node root_once = (root); \
    root_once.nchildren ? PARSER_OK : root_once.token->tk; \
})

void destroy_tree(struct node);

#endif