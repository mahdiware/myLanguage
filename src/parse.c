#include "parse.h"
#include "lex.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define RULE_RHS_LAST 10
#define GRAMMAR_SIZE (sizeof(grammar) / sizeof(*grammar))
#define SKIP_TOKEN(t) ((t) == TK_WSPC || (t) == TK_LCOM || (t) == TK_BCOM)

#define n(_nt) { .nt = NT_##_nt, .is_tk = 0, .is_mt = 0 } // Non-terminal symbol
#define m(_nt) { .nt = NT_##_nt, .is_tk = 0, .is_mt = 1 } // Empty production (epsilon)
#define t(_tm) { .tk = TK_##_tm, .is_tk = 1, .is_mt = 0 } // Terminal symbol
#define no     { .tk = TK_COUNT, .is_tk = 1, .is_mt = 0 } // Placeholder (no symbol)

// The macros above are used to define symbols in a parsing table. They generate initializer expressions for a structure.
// The structure contains information about a symbol, such as whether it is a non-terminal or terminal symbol, and whether it represents an empty production (epsilon).

// Example usage:
// n(Stmt) will expand to { .nt = NT_Stmt, .is_tk = 0, .is_mt = 0 } which represents a non-terminal symbol
// m(Stmt) will expand to { .nt = NT_Stmt, .is_tk = 0, .is_mt = 1 } which represents an empty production
// t(NAME) will expand to { .tk = TK_NAME, .is_tk = 1, .is_mt = 0 } which represents a terminal symbol
// no will expand to { .tk = TK_COUNT, .is_tk = 1, .is_mt = 0 } which represents a placeholder symbol

#define r(_lhs, ...) \
	{ .lhs = NT_##_lhs, .rhs = { __VA_ARGS__, } }, // Production rule with variable number of terminal symbols 

#define r1(_lhs, t1) \
    r(_lhs, no, no, no, no, no, no, no, no, no, no, t1) // Production rule with 1 terminal symbol
#define r2(_lhs, t1, t2) \
    r(_lhs, no, no, no, no, no, no, no, no, no, t1, t2) // Production rule with 2 terminal symbols
#define r3(_lhs, t1, t2, t3) \
    r(_lhs, no, no, no, no, no, no, no, no, t1, t2, t3) // Production rule with 3 terminal symbols
#define r4(_lhs, t1, t2, t3, t4) \
    r(_lhs, no, no, no, no, no, no, no, t1, t2, t3, t4) // Production rule with 4 terminal symbols
#define r5(_lhs, t1, t2, t3, t4, t5) \
    r(_lhs, no, no, no, no, no, no, t1, t2, t3, t4, t5) // Production rule with 5 terminal symbols
#define r6(_lhs, t1, t2, t3, t4, t5, t6) \
    r(_lhs, no, no, no, no, no, t1, t2, t3, t4, t5, t6) // Production rule with 6 terminal symbols
#define r7(_lhs, t1, t2, t3, t4, t5, t6, t7) \
    r(_lhs, no, no, no, no, t1, t2, t3, t4, t5, t6, t7) // Production rule with 7 terminal symbols
#define r8(_lhs, t1, t2, t3, t4, t5, t6, t7, t8) \
    r(_lhs, no, no, no, t1, t2, t3, t4, t5, t6, t7, t8) // Production rule with 7 terminal symbols
#define r9(_lhs, t1, t2, t3, t4, t5, t6, t7, t8, t9) \
    r(_lhs, no, no, t1, t2, t3, t4, t5, t6, t7, t8, t9) // Production rule with 7 terminal symbols
#define r10(_lhs, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10)) \
    r(_lhs, no, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10) // Production rule with 7 terminal symbols

static const struct rule {
    /* left-hand side of production */
    const uint8_t lhs;

    /* array of RULE_RHS_LAST + 1 terms which form the right-hand side */
    const struct term {
        /* a rule RHS term is either a terminal token or a non-terminal */
        union {
            const uint8_t tk;
            const uint8_t nt;
        };

        /* indicates which field of the above union to use */
        const uint8_t is_tk: 1;

        /* indicates that the non-terminal can be matched multiple times */
        const uint8_t is_mt: 1;
    } rhs[RULE_RHS_LAST + 1];
} grammar[] = {
    r3(Unit, t(FBEG), m(Stmt), t(FEND))

    r1(Stmt, n(Assn))        // Statement can be an assignment
    r1(Stmt, n(Prnt))        // Statement can be a print statement
	r1(Stmt, n(Inpt))        // Statement can be an input statement
    r1(Stmt, n(Ctrl))        // Statement can be a control-flow statement
	
	//r1(Iden, t(NAME))
	
	r4(Assn, t(NAME), t(ASSN), t(STRL), t(SCOL))        // Assignment: variable = "string";
    r4(Assn, n(Aexp), t(ASSN), t(STRL), t(SCOL))        // Assignment: array[index] = "string";
    r4(Assn, t(NAME), t(ASSN), t(TRUE), t(SCOL))        // Assignment: variable = true;
    r4(Assn, t(NAME), t(ASSN), t(FALSE), t(SCOL))        // Assignment: variable = false;
    r4(Assn, t(NAME), t(ASSN), t(NULL), t(SCOL))        // Assignment: variable = null;
    r4(Assn, t(NAME), t(ASSN), n(Expr), t(SCOL))        // Assignment: variable = expression;
    r4(Assn, n(Aexp), t(ASSN), n(Expr), t(SCOL))        // Assignment: array[index] = expression;
    
    r3(Prnt, t(PRNT), t(STRL), t(SCOL))        // Print statement: print "string";
    r3(Prnt, t(PRNT), n(Expr), t(SCOL))        // Print statement: print expression;
    r4(Prnt, t(PRNT), t(STRL), n(Expr), t(SCOL))        // Print statement: print "string" expression;
    r5(Prnt, t(PRNT), t(STRL), t(COMA), n(Expr), t(SCOL))
	r3(Inpt, t(INPT), n(Expr), t(SCOL))        // Input statement: input(expression);
	
    r2(Ctrl, n(Cond), m(Elif))        // Control-flow statement: if (condition) { statements } elif (condition) { statements }
    r3(Ctrl, n(Cond), m(Elif), n(Else))        // Control-flow statement: if (condition) { statements } elif (condition) { statements } else { statements }
    r1(Ctrl, n(Dowh))        // Control-flow statement: do { statements } while (condition);
    r1(Ctrl, n(Func))
    r1(Ctrl, n(Whil))        // Control-flow statement: while (condition) { statements }
    
    r7(Func, t(FUNC), t(NAME), t(LPAR), t(RPAR), t(LBRC), m(Stmt), t(RBRC))
	r4(Func, t(NAME), t(LPAR), t(RPAR), t(SCOL))
	
    r5(Cond, t(COND), n(Expr), t(LBRC), m(Stmt), t(RBRC))        // Conditional statement: if (condition) { statements }
    r5(Elif, t(ELIF), n(Expr), t(LBRC), m(Stmt), t(RBRC))        // Elif statement: elif (condition) { statements }
    r4(Else, t(ELSE), t(LBRC), m(Stmt), t(RBRC))        // Else statement: else { statements }
    
    r7(Dowh, t(DOWH), t(LBRC), m(Stmt), t(RBRC), t(WHIL), n(Expr), t(SCOL))        // Do-while loop: do { statements } while (condition);
    r5(Whil, t(WHIL), n(Expr), t(LBRC), m(Stmt), t(RBRC))        // While loop: while (condition) { statements }
    
    
    r1(Atom, t(NAME))        // Atom can be a variable name
    r1(Atom, t(NMBR))        // Atom can be a number
    
    r1(Expr, n(Atom))        // Expression can be an Atom
    r1(Expr, n(Pexp))        // Expression can be a parenthesized expression
    r1(Expr, n(Bexp))        // Expression can be a Boolean expression
    r1(Expr, n(Uexp))        // Expression can be a unary expression
    r1(Expr, n(Texp))        // Expression can be a ternary expression
    r1(Expr, n(Aexp))        // Expression can be an array expression
    
    r3(Pexp, t(LPAR), n(Expr), t(RPAR))        // Parenthesized expression: (expression)
    
    r3(Bexp, n(Expr), t(EQUL), n(Expr))        // Boolean expression: expression == expression
    r3(Bexp, n(Expr), t(NEQL), n(Expr))        // Boolean expression: expression != expression
    r3(Bexp, n(Expr), t(LTHN), n(Expr))        // Boolean expression: expression < expression
    r3(Bexp, n(Expr), t(GTHN), n(Expr))        // Boolean expression: expression > expression
    r3(Bexp, n(Expr), t(LTEQ), n(Expr))        // Boolean expression: expression <= expression
    r3(Bexp, n(Expr), t(GTEQ), n(Expr))        // Boolean expression: expression >= expression
    r3(Bexp, n(Expr), t(CONJ), n(Expr))        // Boolean expression: expression && expression
    r3(Bexp, n(Expr), t(DISJ), n(Expr))        // Boolean expression: expression || expression
    r3(Bexp, n(Expr), t(PLUS), n(Expr))        // Boolean expression: expression + expression
    r3(Bexp, n(Expr), t(MINS), n(Expr))        // Boolean expression: expression - expression
    r3(Bexp, n(Expr), t(MULT), n(Expr))        // Boolean expression: expression * expression
    r3(Bexp, n(Expr), t(DIVI), n(Expr))        // Boolean expression: expression / expression
    r3(Bexp, n(Expr), t(MODU), n(Expr))        // Boolean expression: expression % expression
    
    r2(Uexp, t(PLUS), n(Expr))        // Unary expression: +expression
    r2(Uexp, t(MINS), n(Expr))        // Unary expression: -expression
    r2(Uexp, t(NEGA), n(Expr))        // Unary expression: !expression
    
    r5(Texp, n(Expr), t(QUES), n(Expr), t(COLN), n(Expr))        // Ternary expression: expression ? expression : expression
    
    r4(Aexp, t(NAME), t(LBRA), n(Expr), t(RBRA))        // Array expression: array[index]

};

#undef r1
#undef r2
#undef r3
#undef r4
#undef r5
#undef r6
#undef r7
#undef r8
#undef r9
#undef r10

#undef n
#undef m
#undef t
#undef no

static struct {
    size_t size, allocated;
    struct node *nodes;
} stack;

static void print_stack(void)
{
    static const char *const nts[NT_COUNT] = {
        "Unit",     // Non-terminal represents the start symbol of the grammar - the entire program.
        "Stmt",     // Non-terminal represents a statement.
        "Iden",
        "Assn",     // Non-terminal represents an assignment statement.
        "Prnt",     // Non-terminal represents a print statement.
        "Inpt",     // Non-terminal represents an input statement.
        "Ctrl",     // Non-terminal represents a control statement.
        "Func",
        "Cond",     // Non-terminal represents a conditional statement.
        "Elif",     // Non-terminal represents an "elif" branch of a conditional statement.
        "Else",     // Non-terminal represents an "else" branch of a conditional statement.
        "Dowh",     // Non-terminal represents a "do-while" loop statement.
        "Whil",     // Non-terminal represents a "while" loop statement.
        "Atom",     // Non-terminal represents an Atomic expression.
        "Expr",     // Non-terminal represents an expression.
        "Pexp",     // Non-terminal represents a Parenthesize expression.
        "Bexp",     // Non-terminal represents a Boolean expression.
        "Uexp",     // Non-terminal represents a unary expression.
        "Texp",     // Non-terminal represents a ternary expression.
        "Aexp",     // Non-terminal represents an arithmetic expression.
    };


    for (size_t i = 0; i < stack.size; ++i) {
        const struct node *const node = &stack.nodes[i];

        if (node->nchildren) {
        	printf(YELLOW("%s "), nts[node->nt]);
            //printf(YELLOW("%d "), node->nt);
        } else if (node->token->tk == TK_FBEG) {
            printf(GREEN("^ "));
        } else if (node->token->tk == TK_FEND) {
            printf(GREEN("$ "));
        } else {
            const ptrdiff_t len = node->token->end - node->token->beg;
            printf(GREEN(" %.*s "), (int) len, node->token->beg);
        }
    }

    puts("");
}

static void destroy_node(const struct node *const node)
{
    // Recursively destroy the children nodes
    if (node->nchildren) {
        for (size_t child_idx = 0; child_idx < node->nchildren; ++child_idx) {
            destroy_node(node->children[child_idx]);
        }

        // Free memory allocated for children nodes
        free(node->children[0]);
        free(node->children);
    }
}

static void deallocate_stack(void)
{
    // Free memory allocated for the stack nodes
    free(stack.nodes);
    stack.nodes = NULL;
    stack.size = 0;
    stack.allocated = 0;
}

static void destroy_stack(void)
{
    // Destroy all nodes in the stack
    for (size_t node_idx = 0; node_idx < stack.size; ++node_idx) {
        destroy_node(&stack.nodes[node_idx]);
    }

    // Deallocate the stack memory
    deallocate_stack();
}

static inline int term_eq_node(const struct term *const term, const struct node *const node)
{
    const int node_is_leaf = node->nchildren == 0;

    if (term->is_tk == node_is_leaf) {
        if (node_is_leaf) {
            // Check if the terminal token matches the node's token
            return term->tk == node->token->tk;
        } else {
            // Check if the non-terminal token matches the node's non-terminal
            return term->nt == node->nt;
        }
    }

    return 0;
}

static size_t match_rule(const struct rule *const rule, size_t *const at)
{
    const struct term *prev = NULL;
    const struct term *term = &rule->rhs[RULE_RHS_LAST];
    ssize_t st_idx = stack.size - 1;

    do {
        if (term_eq_node(term, &stack.nodes[st_idx])) {
            // Match the terms in reverse order
            prev = term->is_mt ? term : NULL;
            --term, --st_idx;
        } else if (prev && term_eq_node(prev, &stack.nodes[st_idx])) {
            // Match the previous term again
            --st_idx;
        } else if (term->is_mt) {
            // Match an empty term
            prev = NULL;
            --term;
        } else {
            // Failed to match the terms
            term = NULL;
            break;
        }
    } while (st_idx >= 0 && !(term->is_tk && term->tk == TK_COUNT));

    const int reached_eor = term && term->is_tk && term->tk == TK_COUNT;
    const size_t reduction_size = stack.size - st_idx - 1;

    return reached_eor && reduction_size ?
        (*at = st_idx + 1, reduction_size) : 0;
}

static inline int shift(const struct token *const token)
{
    if (stack.size >= stack.allocated) {
        // Expand the stack if it's full
        stack.allocated = (stack.allocated ?: 1) * 8;

        struct node *const tmp = realloc(stack.nodes,
            stack.allocated * sizeof(struct node));

        if (!tmp) {
            return PARSE_NOMEM;
        }

        stack.nodes = tmp;
    }

    // Shift the token onto the stack
    stack.nodes[stack.size++] = (struct node) {
        .nchildren = 0,
        .token = token,
    };

    return PARSE_OK;
}

static inline bool should_shift_pre(const struct rule *const rule, const struct token *const tokens, size_t *const token_idx)
{
    if (rule->lhs == NT_Unit) {
        // No need to shift for NT_Unit
        return false;
    }

    while (SKIP_TOKEN(tokens[*token_idx].tk)) {
        // Skip tokens that need to be ignored
        ++*token_idx;
    }

    const struct token *const ahead = &tokens[*token_idx];
	const struct token *const aback = &tokens[*token_idx-3];
	
    if (rule->lhs == NT_Bexp && ahead->tk >= TK_EQUL && ahead->tk <= TK_MODU) {
        /*
            Check whether the operator ahead has a lower precedence. If it has,
            let the parser shift it before applying the Bexp reduction.
        */
        const uint8_t precedence[TK_MODU - TK_EQUL + 1] = {4, 4, 3, 3, 3, 3, 5, 6, 2, 2, 1, 1, 1,};
		
        const uint8_t p1 = precedence[rule->rhs[RULE_RHS_LAST - 1].tk - TK_EQUL];
        const uint8_t p2 = precedence[ahead->tk - TK_EQUL];

        if (p2 < p1) {
            return true;
        }
    } else if (rule->lhs == NT_Atom && rule->rhs[RULE_RHS_LAST].tk == TK_NAME) {
        /*
            Do not allow the left side of an assignment or an array name to
            escalate to Expr.
        */
        if (ahead->tk == TK_ASSN || ahead->tk == TK_LBRA || aback->tk == TK_FUNC || ahead->tk == TK_LPAR) {
            return true;
        }
        //printf("sorry: %d", aback->tk);
    } else if (rule->lhs == NT_Expr && rule->rhs[RULE_RHS_LAST].nt == NT_Aexp) {
        /*
            Do not allow an Aexp on the left side of an assignment to escalate
            to Expr.
        */
        if (ahead->tk == TK_ASSN) {
            return true;
        }
    }

    return false;
}

static inline bool should_shift_post(const struct rule *const rule, const struct token *const tokens, size_t *const token_idx)
{
    if (rule->lhs == NT_Unit) {
        return false;
    }

    while (SKIP_TOKEN(tokens[*token_idx].tk)) {
        ++*token_idx;
    }

    const struct token *const ahead = &tokens[*token_idx];

    if (rule->lhs == NT_Cond || rule->lhs == NT_Elif) {
        /* swallow the next "elif" or "else" in order to parse the whole chain */
        if (ahead->tk == TK_ELIF || ahead->tk == TK_ELSE) {
            return true;
        }
    }

    return false;
}

static int reduce(const struct rule *const rule, const size_t at, const size_t size)
{
    struct node *const child_nodes = malloc(size * sizeof(struct node));
	//struct node *const child_nodes = calloc(size, sizeof(struct node));

    if (!child_nodes) {
        return PARSE_NOMEM;
    }

    struct node *const reduce_at = &stack.nodes[at];
    struct node **const old_children = reduce_at->children;
    reduce_at->children = malloc(size * sizeof(struct node *)) ?: old_children;
	//reduce_at->children = calloc(size, sizeof(struct node *)) ?: old_children;

    if (reduce_at->children == old_children) {
        return free(child_nodes), PARSE_NOMEM;
    }

    for (size_t child_idx = 0, st_idx = at;
        st_idx < stack.size;
        ++st_idx, ++child_idx) {

        child_nodes[child_idx] = stack.nodes[st_idx];
        reduce_at->children[child_idx] = &child_nodes[child_idx];
    }

    child_nodes[0].children = old_children;
    reduce_at->nchildren = size;
    reduce_at->nt = rule->lhs;
    stack.size = at + 1;
    return PARSE_OK;
}

struct node parse(const struct token *const tokens, const size_t ntokens)
{
    static const struct token
        reject = { .tk = PARSE_REJECT },
        nomem  = { .tk = PARSE_NOMEM  };

    static const struct node
        err_reject = { .nchildren = 0, .token = &reject },
        err_nomem  = { .nchildren = 0, .token = &nomem  };

    #define SHIFT_OR_NOMEM(t) \
        if (shift(t)) { \
            puts(RED("Out of memory on shift!")); \
            return destroy_stack(), err_nomem; \
        }

    #define REDUCE_OR_NOMEM(r, a, s) \
        if (reduce(r, a, s)) { \
            puts(RED("Out of memory on reduce!")); \
            return destroy_stack(), err_nomem; \
        }

    for (size_t token_idx = 0; token_idx < ntokens; ) {
        if (SKIP_TOKEN(tokens[token_idx].tk)) {
            ++token_idx;
            continue;
        }

        SHIFT_OR_NOMEM(&tokens[token_idx++]);
        if(development == 1){
        	printf(CYAN("Shift: ")), print_stack();
		}
		
        try_reduce_again:;
        const struct rule *rule = grammar;

        do {
            size_t reduction_at, reduction_size;

            if ((reduction_size = match_rule(rule, &reduction_at))) {
                const bool do_shift = should_shift_pre(rule, tokens, &token_idx);

                if (!do_shift) {
                    REDUCE_OR_NOMEM(rule, reduction_at, reduction_size);
                    if(development == 1){
                    	const ptrdiff_t rule_number = rule - grammar + 1;
                    	printf(ORANGE("Red%02td: "), rule_number), print_stack();
                    }
                }

                if (do_shift || should_shift_post(rule, tokens, &token_idx)) {
                    SHIFT_OR_NOMEM(&tokens[token_idx++]);
                    if(development == 1){
                    	printf(CYAN("Shift: ")), print_stack();
                    }
                }

                goto try_reduce_again;
            }
        } while (++rule != grammar + GRAMMAR_SIZE);
    }

    #undef SHIFT_OR_NOMEM
    #undef REDUCE_OR_NOMEM

    const int accepted = stack.size == 1 &&
        stack.nodes[0].nchildren && stack.nodes[0].nt == NT_Unit;
	
	if(development == 1){
    	printf(accepted ? GREEN("ACCEPT ") : RED("REJECT ")), print_stack();
	}
	
    if (accepted) {
        const struct node ret = stack.nodes[0];
        return deallocate_stack(), ret;
    } else {
        return destroy_stack(), err_reject;
    }
}

void destroy_tree(const struct node root)
{
    destroy_node(&root);
}
