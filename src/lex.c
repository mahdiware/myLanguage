#include "lex.h"

#include <stdio.h>
#include <stdlib.h>

enum {
    STS_ACCEPT,
    STS_REJECT,
    STS_HUNGRY,
};

#define TR(st, tr) (*s = (st), (STS_##tr))
#define REJECT TR(0, REJECT)

#define IS_ALPHA(c)  (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define IS_DIGIT(c)  ((c) >= '0' && (c) <= '9')
#define IS_FLOAT(c)  (IS_DIGIT(c) || (c) == '.')
#define IS_ALNUM(c)  (IS_ALPHA(c) || IS_DIGIT(c))
#define IS_WSPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')


#define TOKEN_DEFINE(token, str) \
static uint8_t token(const uint8_t c, uint8_t *const s) \
{ \
	int count = 0; \
	for (int i = 0; str[i] != '\0'; i++) { \
        count++; \
    } \
	uint8_t i = 0; \
	for(i = 0; i <= (count-2); ++i){ \
		if(i == *s){ \
			return c == (str)[i] ? TR(i+1, HUNGRY) : REJECT; \
		} \
	} \
	if(count-1 == *s){ \
		return c == (str)[i] ? TR(i+1, ACCEPT) : REJECT; \
	} \
	i++; \
	if(count == *s){ \
		return REJECT; \
	} \
	abort(); \
} \

static uint8_t tk_name(const uint8_t c, uint8_t *const s)
{
    enum {
        tk_name_begin,
        tk_name_accum,
    };

    switch (*s) {
    case tk_name_begin:
        return IS_ALPHA(c) || (c == '_') ? TR(tk_name_accum, ACCEPT) : REJECT;

    case tk_name_accum:
        return IS_ALNUM(c) || (c == '_') ? STS_ACCEPT : REJECT;
    }

    abort();
}

static uint8_t tk_nmbr(const uint8_t c, uint8_t *const s)
{
    (void) s;
    return IS_DIGIT(c) || IS_FLOAT(c) ? STS_ACCEPT : STS_REJECT;
}

static uint8_t tk_strl(const uint8_t c, uint8_t *const s)
{
    enum {
        tk_strl_begin,
        tk_strl_accum,
        tk_strl_end,
    };

    switch (*s) {
    case tk_strl_begin:
        return c == '"' ? TR(tk_strl_accum, HUNGRY) : REJECT;

    case tk_strl_accum:
        return c != '"' ? STS_HUNGRY : TR(tk_strl_end, ACCEPT);

    case tk_strl_end:
        return REJECT;
    }

    abort();
}

static uint8_t tk_wspc(const uint8_t c, uint8_t *const s)
{
    enum {
        tk_wspc_begin,
        tk_wspc_accum,
    };

    switch (*s) {
    case tk_wspc_begin:
        return IS_WSPACE(c) ? TR(tk_wspc_accum, ACCEPT) : REJECT;

    case tk_wspc_accum:
        return IS_WSPACE(c) ? STS_ACCEPT : REJECT;
    }

    abort();
}

static uint8_t tk_lcom(const uint8_t c, uint8_t *const s)
{
    enum {
        tk_lcom_begin,
        tk_lcom_first_slash,
        tk_lcom_accum,
        tk_lcom_end
    };

    switch (*s) {
    case tk_lcom_begin:
        return c == '/' ? TR(tk_lcom_first_slash, HUNGRY) : REJECT;

    case tk_lcom_first_slash:
        return c == '/' ? TR(tk_lcom_accum, HUNGRY) : REJECT;

    case tk_lcom_accum:
        return c == '\n' || c == '\r' ? TR(tk_lcom_end, ACCEPT) : STS_HUNGRY;

    case tk_lcom_end:
        return REJECT;
    }

    abort();
}

static uint8_t tk_bcom(const uint8_t c, uint8_t *const s)
{
    enum {
        tk_bcom_begin,
        tk_bcom_open_slash,
        tk_bcom_accum,
        tk_bcom_close_star,
        tk_bcom_end
    };

    switch (*s) {
    case tk_bcom_begin:
        return c == '/' ? TR(tk_bcom_open_slash, HUNGRY) : REJECT;

    case tk_bcom_open_slash:
        return c == '*' ? TR(tk_bcom_accum, HUNGRY) : REJECT;

    case tk_bcom_accum:
        return c != '*' ? STS_HUNGRY : TR(tk_bcom_close_star, HUNGRY);

    case tk_bcom_close_star:
        return c == '/' ? TR(tk_bcom_end, ACCEPT) : TR(tk_bcom_accum, HUNGRY);

    case tk_bcom_end:
        return REJECT;
    }

    abort();
}

TOKEN_DEFINE(tk_lpar, "(")
TOKEN_DEFINE(tk_rpar, ")")
TOKEN_DEFINE(tk_lbra, "[")
TOKEN_DEFINE(tk_rbra, "]")
TOKEN_DEFINE(tk_lbrc, "{")
TOKEN_DEFINE(tk_rbrc, "}")
TOKEN_DEFINE(tk_cond, "if")
TOKEN_DEFINE(tk_elif, "elif")
TOKEN_DEFINE(tk_else, "else")
TOKEN_DEFINE(tk_dowh, "do")
TOKEN_DEFINE(tk_whil, "while")
TOKEN_DEFINE(tk_assn, "=")
TOKEN_DEFINE(tk_equl, "==")
TOKEN_DEFINE(tk_neql, "!=")
TOKEN_DEFINE(tk_lthn, "<")
TOKEN_DEFINE(tk_gthn, ">")
TOKEN_DEFINE(tk_lteq, "<=")
TOKEN_DEFINE(tk_gteq, ">=")
TOKEN_DEFINE(tk_conj, "&&")
TOKEN_DEFINE(tk_disj, "||")
TOKEN_DEFINE(tk_plus, "+")
TOKEN_DEFINE(tk_mins, "-")
TOKEN_DEFINE(tk_mult, "*")
TOKEN_DEFINE(tk_divi, "/")
TOKEN_DEFINE(tk_modu, "%")
TOKEN_DEFINE(tk_nega, "!")
TOKEN_DEFINE(tk_prnt, "print")
TOKEN_DEFINE(tk_inpt, "input")
TOKEN_DEFINE(tk_scol, ";")
TOKEN_DEFINE(tk_ques, "?")
TOKEN_DEFINE(tk_coln, ":")
//TOKEN_DEFINE(tk_coma, ",")

static uint8_t (*const token_funcs[TK_COUNT])(const uint8_t, uint8_t *const) = {
    tk_name,    // Function to recognize names/identifiers
    tk_nmbr,    // Function to recognize numbers
    tk_strl,    // Function to recognize string literals
    tk_wspc,    // Function to recognize whitespace
    tk_lcom,    // Function to recognize line comments
    tk_bcom,    // Function to recognize block comments
    tk_lpar,    // Function to recognize left parentheses
    tk_rpar,    // Function to recognize right parentheses
    tk_lbra,    // Function to recognize left brackets
    tk_rbra,    // Function to recognize right brackets
    tk_lbrc,    // Function to recognize left braces
    tk_rbrc,    // Function to recognize right braces
    tk_cond,    // Function to recognize conditional keyword "if"
    tk_elif,    // Function to recognize conditional keyword "elif"
    tk_else,    // Function to recognize conditional keyword "else"
    tk_dowh,    // Function to recognize loop keyword "do"
    tk_whil,    // Function to recognize loop keyword "while"
    tk_assn,    // Function to recognize assignment operator "="
    tk_equl,    // Function to recognize comparison operator "=="
    tk_neql,    // Function to recognize comparison operator "!="
    tk_lthn,    // Function to recognize comparison operator "<"
    tk_gthn,    // Function to recognize comparison operator ">"
    tk_lteq,    // Function to recognize comparison operator "<="
    tk_gteq,    // Function to recognize comparison operator ">="
    tk_conj,    // Function to recognize logical operator "&&"
    tk_disj,    // Function to recognize logical operator "||"
    tk_plus,    // Function to recognize arithmetic operator "+"
    tk_mins,    // Function to recognize arithmetic operator "-"
    tk_mult,    // Function to recognize arithmetic operator "*"
    tk_divi,    // Function to recognize arithmetic operator "/"
    tk_modu,    // Function to recognize arithmetic operator "%"
    tk_nega,    // Function to recognize negation operator "!"
    tk_prnt,    // Function to recognize print statement keyword "print"
    tk_inpt,    // Function to recognize input statement keyword "input"
    tk_scol,    // Function to recognize semicolon ";"
    tk_ques,    // Function to recognize ternary operator "?"
    tk_coln,    // Function to recognize colon ":"
	//    tk_coma,  // Function to recognize comma ","
};

static inline int push_token(struct token **const tokens,
    size_t *const ntokens, size_t *const allocated, const uint8_t token,
    const uint8_t *const beg, const uint8_t *const end)
{
    if (*ntokens >= *allocated) {
        // If the number of tokens exceeds the allocated memory, reallocate memory to accommodate more tokens.
        *allocated = (*allocated ?: 1) * 8;

        struct token *const tmp =
            realloc(*tokens, *allocated * sizeof(struct token));

        if (!tmp) {
            // If memory reallocation fails, free the existing tokens, set them to NULL, and return an error code indicating no memory.
            return free(*tokens), *tokens = NULL, LEX_NOMEM;
        }

        *tokens = tmp;
    }

    // Add the new token to the tokens array.
    (*tokens)[(*ntokens)++] = (struct token) {
        .beg = beg,
        .end = end,
        .tk = token
    };

    return LEX_OK;
}

int lex(const uint8_t *const input, const size_t size,
    struct token **const tokens, size_t *const ntokens)
{
    // Array to store the previous and current states of each token type.
    static struct {
        uint8_t prev, curr;
    } statuses[TK_COUNT] = {
        [0 ... TK_COUNT - 1] = { STS_HUNGRY, STS_REJECT }
    };

    // Array to store the current states of each token type.
    uint8_t states[TK_COUNT] = {0};

    // Variables to keep track of the current prefix being analyzed.
    const uint8_t *prefix_beg = input, *prefix_end = input;

    // Variable to store the accepted token type.
    uint8_t accepted_token;

    // Variable to keep track of the allocated memory for tokens.
    size_t allocated = 0;

    // Initialize tokens array and number of tokens.
    *tokens = NULL, *ntokens = 0;

    // Macro to push a token to the tokens array or return a no memory error.
    #define PUSH_OR_NOMEM(tk, beg, end) \
        if (push_token(tokens, ntokens, &allocated, (tk), (beg), (end))) { \
            return LEX_NOMEM; \
        }

    // Macro to iterate over each token type.
    #define foreach_tk \
        for (uint8_t tk = 0; tk < TK_COUNT; ++tk)

    // Push the initial token indicating the start of the file.
    PUSH_OR_NOMEM(TK_FBEG, NULL, NULL);

    while (prefix_end < input + size) {
        int did_accept = 0;

        foreach_tk {
            // Update the current state of each token type based on the current prefix.
            if (statuses[tk].prev != STS_REJECT) {
                statuses[tk].curr = token_funcs[tk](*prefix_end, &states[tk]);
            }

            if (statuses[tk].curr != STS_REJECT) {
                did_accept = 1;
            }
        }

        if (did_accept) {
            // Move to the next character in the input.
            prefix_end++;

            // Update the previous state of each token type.
            foreach_tk {
                statuses[tk].prev = statuses[tk].curr;
            }
        } else {
            // No token type accepted the current prefix, determine the accepted token type.

            accepted_token = TK_COUNT;

            foreach_tk {
                if (statuses[tk].prev == STS_ACCEPT) {
                    accepted_token = tk;
                }

                // Reset the previous and current states of each token type.
                statuses[tk].prev = STS_HUNGRY;
                statuses[tk].curr = STS_REJECT;
            }

            // Push the accepted token to the tokens array.
            PUSH_OR_NOMEM(accepted_token, prefix_beg, prefix_end);

            if (accepted_token == TK_COUNT) {
                // If no token type was accepted, increase the end position of the previous token by one and return an error indicating an unknown token.
                (*tokens)[*ntokens - 1].end++;
                return LEX_UNKNOWN_TOKEN;
            }

            // Update the prefix positions.
            prefix_beg = prefix_end;
        }
    }

    // Process the remaining prefixes after reaching the end of the input.

    accepted_token = TK_COUNT;

    foreach_tk {
        if (statuses[tk].curr == STS_ACCEPT) {
            accepted_token = tk;
        }

        // Reset the previous and current states of each token type.
        statuses[tk].prev = STS_HUNGRY;
        statuses[tk].curr = STS_REJECT;
    }

    // Push the accepted token to the tokens array.
    PUSH_OR_NOMEM(accepted_token, prefix_beg, prefix_end);

    if (accepted_token == TK_COUNT) {
        // If no token type was accepted, return an error indicating an unknown token.
        return LEX_UNKNOWN_TOKEN;
    }

    // Push the final token indicating the end of the file.
    PUSH_OR_NOMEM(TK_FEND, NULL, NULL);

    // Return the success code.
    return LEX_OK;

    // Undefine the macros.
    #undef PUSH_OR_NOMEM
    #undef foreach_tk
}
