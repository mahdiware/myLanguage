#pragma once

#ifndef LEX_H
#define LEX_H

#include <stdint.h>
#include <stddef.h>

#define COLOURED(s, b, c) "\033[" #b ";" #c "m" s "\033[0m"
#define GRAY(s)           COLOURED(s, 0, 37)
#define RED(s)            COLOURED(s, 1, 31)
#define GREEN(s)          COLOURED(s, 1, 32)
#define YELLOW(s)         COLOURED(s, 1, 33)
#define ORANGE(s)         COLOURED(s, 1, 34)
#define CYAN(s)           COLOURED(s, 1, 36)
#define WHITE(s)          COLOURED(s, 1, 37)

enum {
    TK_NAME,   // Token for a name
    TK_NMBR,   // Token for a number
    TK_STRL,   // Token for a string literal
    TK_WSPC,   // Token for whitespace
    TK_LCOM,   // Token for a line comment
    TK_BCOM,   // Token for a block comment
    TK_LPAR,   // Token for a left parenthesis '('
    TK_RPAR,   // Token for a right parenthesis ')'
    TK_LBRA,   // Token for a left bracket '['
    TK_RBRA,   // Token for a right bracket ']'
    TK_LBRC,   // Token for a left brace '{'
    TK_RBRC,   // Token for a right brace '}'
    TK_COND,   // Token for the 'if' keyword
    TK_ELIF,   // Token for the 'elif' keyword
    TK_ELSE,   // Token for the 'else' keyword
    TK_DOWH,   // Token for the 'do-while' keyword
    TK_WHIL,   // Token for the 'while' keyword
    TK_ASSN,   // Token for the assignment operator '='
    TK_EQUL,   // Token for the equality operator '=='
    TK_NEQL,   // Token for the inequality operator '!='
    TK_LTHN,   // Token for the less than operator '<'
    TK_GTHN,   // Token for the greater than operator '>'
    TK_LTEQ,   // Token for the less than or equal to operator '<='
    TK_GTEQ,   // Token for the greater than or equal to operator '>='
    TK_CONJ,   // Token for the logical conjunction operator '&&'
    TK_DISJ,   // Token for the logical disjunction operator '||'
    TK_PLUS,   // Token for the plus operator '+'
    TK_MINS,   // Token for the minus operator '-'
    TK_MULT,   // Token for the multiplication operator '*'
    TK_DIVI,   // Token for the division operator '/'
    TK_MODU,   // Token for the modulo operator '%'
    TK_NEGA,   // Token for the logical negation operator '!'
    TK_FUNC,	// Token for the 'function' keyword
    TK_PRNT,   // Token for the 'print' keyword
    TK_INPT,   // Token for the 'input' keyword
    TK_SCOL,   // Token for the semicolon ';'
    TK_QUES,   // Token for the question mark '?'
    TK_COLN,   // Token for the colon ':'
    TK_COMA,   // Token for the comma ','
    TK_COUNT,  // Total number of tokens
    TK_FBEG,   // Token for the beginning of a file
    TK_FEND,   // Token for the end of a file
};

struct token {
    const uint8_t *beg, *end;   // Pointers to the beginning and end of the token in the input
    uint8_t tk;                 // Token type
};

int lex(const uint8_t *, size_t, struct token **, size_t *);

enum {
    LEX_OK,                 // Lexical analysis completed successfully
    LEX_NOMEM,              // Out of memory error during lexical analysis
    LEX_UNKNOWN_TOKEN,      // Unknown token encountered during lexical analysis
};

#endif