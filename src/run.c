#include "lex.h"
#include "Utils.h"
#include "run.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

static void run_stmt(const struct node *const);     // Function to execute a statement node
static void run_assn(const struct node *const);     // Function to execute an assignment node
static void run_func(const struct node *const);
static void run_prnt(const struct node *const);     // Function to execute a print node
static void run_inpt(const struct node *const);     // Function to execute a inpt node
static void run_ctrl(const struct node *const);     // Function to execute a control node
static int eval_atom(const struct node *const);     // Function to evaluate an atomic expression node
static int eval_expr(const struct node *const);     // Function to evaluate a generic expression node
static int eval_pexp(const struct node *const);     // Function to evaluate a parenthesized expression node
static int eval_bexp(const struct node *const);     // Function to evaluate a boolean expression node
static int eval_uexp(const struct node *const);     // Function to evaluate a unary expression node
static int eval_texp(const struct node *const);     // Function to evaluate a ternary expression node
static int eval_aexp(const struct node *const);     // Function to evaluate an arithmetic expression node

enum {
	VAR_INT,			//Variable type integer
	VAR_STR,			//Variable type string
	VAR_FLT,			//Variable type float
};

static struct {
    size_t size;                                 // Current number of variables stored in the varstore

    struct {
        const uint8_t *beg;                       // Beginning memory address of the variable name
        ptrdiff_t len;                            // Length of the variable name
        size_t array_size;                        // Size of the array, if the variable is an array
        int *values;                              // Pointer to the array of variable values
        uint8_t type;							 // Type of VarStore
        const uint8_t *strbeg;                   // Beginning memory address of the variable Value
        ptrdiff_t strlen;                         // Length of the variable Value
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

void run(const struct node *const unit)
{
    // Execute each statement in the program
    for (size_t stmt_idx = 1; stmt_idx < unit->nchildren - 1; ++stmt_idx) {
        run_stmt(unit->children[stmt_idx]);
    }
    // Free the memory associated with variable values
    for (size_t var_idx = 0; var_idx < varstore.size; ++var_idx) {
        free(varstore.vars[var_idx].values);
    }
    // Reset the size of the variable store
    varstore.size = 0;
}

static void run_stmt(const struct node *const stmt)
{
    switch (stmt->children[0]->nt) {
        case NT_Assn:                                  // If the statement is an assignment statement
            run_assn(stmt->children[0]);               // Execute the run_assn function for the assignment
            break;
        case NT_Prnt:                                  // If the statement is a print statement
            run_prnt(stmt->children[0]);               // Execute the run_prnt function for the print statement
            break;		
        case NT_Inpt:                                  // If the statement is an input statement
            run_inpt(stmt->children[0]);               // Execute the run_inpt function for the input statement
            break;
        case NT_Ctrl:                                  // If the statement is a control statement
            run_ctrl(stmt->children[0]);               // Execute the run_ctrl function for the control statement
            break;
        default:
            abort();                                   // If the statement is of an unknown type, abort the program
    }
}

static void run_assn(const struct node *const assn)
{
	
    // Check if the left-hand side of the assignment is an array expression
    const int lhs_is_aexp = assn->children[0]->nchildren;
	
	// Evaluate the array index if present, or set it to 0
    const int array_idx = lhs_is_aexp ? eval_expr(assn->children[0]->children[2]) : 0;
	
    // Get the beginning position and length of the variable name or array name
    const uint8_t *const beg = lhs_is_aexp ?
        assn->children[0]->children[0]->token->beg :
        assn->children[0]->token->beg;
	
    const ptrdiff_t len = lhs_is_aexp ?
        assn->children[0]->children[0]->token->end - beg :
        assn->children[0]->token->end - beg;
	
    size_t var_idx;
	
    // Search for the variable in the variable store
    for (var_idx = 0; var_idx < varstore.size; ++var_idx) {
        // Check if the variable name matches and has the same length
        if (varstore.vars[var_idx].len == len && !memcmp(varstore.vars[var_idx].beg, beg, len)) {
            const size_t array_size = varstore.vars[var_idx].array_size;
            // Check if the variable is a non-array variable
            if (!array_size) {
                fprintf(stderr, "warn: a previous reallocation has failed, "
                    "assignment has no effect\n");

                return;
            }
            //node string value
			const struct node *const strl = assn->children[2];
			const uint8_t *const str_beg = strl->token->beg + 1;
			const uint8_t *const str_end = strl->token->end - 1;
			const ptrdiff_t str_len = str_end - str_beg;
		        	
            // Check if the array index is within the bounds
            if (array_idx >= 0 && array_idx < array_size) {
            	if(assn->children[2]->token->tk == TK_STRL){
            		varstore.vars[var_idx].strbeg = str_beg;
        			varstore.vars[var_idx].strlen = str_len;
        			varstore.vars[var_idx].type = VAR_STR;
					// Evaluate the expression on the right-hand side and assign the value
                	varstore.vars[var_idx].values[array_idx] = 0;
            	}else{
            		varstore.vars[var_idx].strlen = 0;
            		varstore.vars[var_idx].type = VAR_INT;
            		varstore.vars[var_idx].strbeg = NULL;
                	// Evaluate the expression on the right-hand side and assign the value
                	varstore.vars[var_idx].values[array_idx] =
                    	eval_expr(assn->children[2]);
				}
                return;
            }
            // Check if the array index is larger than the current array size
            else if (array_idx >= 0) {
                // Calculate the new size of the array by doubling it
                const size_t new_size = (array_idx + 1) * 2;

                // Reallocate memory for the array and store the new size
                int *const tmp = realloc(
                    varstore.vars[var_idx].values, new_size * sizeof(int));
                if (!tmp) {
                    free(varstore.vars[var_idx].values);
                    varstore.vars[var_idx].array_size = 0;
                    varstore.vars[var_idx].values = NULL;
                    perror("realloc");
                    return;
                }

                varstore.vars[var_idx].values = tmp;
                varstore.vars[var_idx].array_size = new_size;
				
				if(assn->children[2]->token->tk == TK_STRL){
					varstore.vars[var_idx].strbeg = str_beg;
        			varstore.vars[var_idx].strlen = str_len;
        			varstore.vars[var_idx].type = VAR_STR;
					// Evaluate the expression on the right-hand side and assign the value
                	varstore.vars[var_idx].values[array_idx] = 0;
				}else{
					varstore.vars[var_idx].strbeg = NULL;
					varstore.vars[var_idx].strlen = 0;
					varstore.vars[var_idx].type = VAR_INT;
					// Evaluate the expression on the right-hand side and assign the value
                	varstore.vars[var_idx].values[array_idx] =
                    	eval_expr(assn->children[2]);
				}
                return;
            } else {
                fprintf(stderr, "warn: negative array offset\n");
                return;
            }
        }
    }
	
    // If the variable was not found in the variable store
    if (var_idx < VAR_CAPACITY) {
        // Check if the array index is negative
        if (array_idx < 0) {
            fprintf(stderr, "warn: negative array offset\n");
            return;
        }

        // Allocate memory for the variable value array
        varstore.vars[var_idx].beg = beg;
        varstore.vars[var_idx].len = len;
        varstore.vars[var_idx].values = malloc((array_idx + 1) * sizeof(char *));
        varstore.vars[var_idx].array_size = 0;
		
		
		const struct node *const strl = assn->children[2];

        const uint8_t *const strbeg = strl->token->beg + 1;
        const uint8_t *const strend = strl->token->end - 1;
        const ptrdiff_t strlen = strend - strbeg;
		
		if (!varstore.vars[var_idx].values) {
            perror("malloc");
            return;
        }
		
        varstore.vars[var_idx].array_size = array_idx + 1;
		
		if(assn->children[2]->token->tk == TK_STRL){
			// Evaluate the expression on the right-hand side and assign the value
        	varstore.vars[var_idx].strbeg = strbeg;
        	varstore.vars[var_idx].strlen = strlen;
        	varstore.vars[var_idx].type = VAR_STR;
        	varstore.vars[var_idx].values[array_idx] = 0;
			
		}else{
			varstore.vars[var_idx].strlen = 0;
			varstore.vars[var_idx].type = VAR_INT;
			varstore.vars[var_idx].strbeg = NULL;
        	// Evaluate the expression on the right-hand side and assign the value
        	varstore.vars[var_idx].values[array_idx] = eval_expr(assn->children[2]);
		}
		varstore.size++;
    } else {
        fprintf(stderr, "warn: varstore exhausted, assignment has no effect\n");
    }
}

static void run_func(const struct node *const func)
{
	if(func->children[0]->token->tk == TK_FUNC && func->children[1]->token->tk == TK_NAME){
		const uint8_t *beg = func->children[1]->token->beg;
		const ptrdiff_t len = func->children[1]->token->end - func->children[1]->token->beg;
		
		struct node *stmt = func->children[5];
		
		size_t fun_idx;
		for (fun_idx = 0; fun_idx < function.size; ++fun_idx) {
			
		}
		
		if(fun_idx < VAR_CAPACITY){
			function.get[fun_idx].beg = beg;
			function.get[fun_idx].len = len;
			function.get[fun_idx].children = stmt;			
		}
		
		function.size++;
	}else if(func->children[0]->token->tk == TK_NAME && func->children[1]->token->tk == TK_LPAR){
		const uint8_t *beg = func->children[0]->token->beg;
		const ptrdiff_t len = func->children[0]->token->end - func->children[0]->token->beg;
				
		for (size_t idx = 0; idx < function.size; ++idx) {
	        if (function.get[idx].len == len && !memcmp(function.get[idx].beg, beg, len)) {
	            // Check if the variable is an array
	            while (function.get[idx].children->nchildren) {
                	run_stmt(function.get[idx].children++);
                }
	            
	        }
	    }
	}
}

static void run_prnt(const struct node *const prnt)
{
    
	// If there are 3 children, it means it's a print statement without an expression
	
    if (prnt->nchildren == 3) {
    	if (prnt->children[1]->token->tk == TK_STRL) {
            const struct node *const strl = prnt->children[1];
            
        	// Extract the string from the token
        	const uint8_t *const beg = strl->token->beg + 1;
        	const uint8_t *const end = strl->token->end - 1;
        	const ptrdiff_t len = end - beg;
        	
        	char *strprint = (char *)malloc((len+1) * sizeof(char));
	        sprintf(strprint, "%.*s", len, beg);
        	// Print the string
            printf("%s", replace(strprint, "\\n", "\n"));
            //printf("%.*s", len, beg);
        } else {
        	
        	if(prnt->children[1]->children[0]->children[0]->token->tk==TK_NAME){
        		const uint8_t *const bbeg = prnt->children[1]->children[0]->children[0]->token->beg;
        		const ptrdiff_t blen = prnt->children[1]->children[0]->children[0]->token->end - bbeg;
	        	for (size_t idx = 0; idx < varstore.size; ++idx) {
			        if (varstore.vars[idx].len == blen && !memcmp(varstore.vars[idx].beg, bbeg, blen)) {
			            if (varstore.vars[idx].array_size && varstore.vars[idx].strbeg != NULL) {
			                //printf("%.*s\n", varstore.vars[idx].strlen, varstore.vars[idx].strbeg);
			                
			                char *strprint = (char *)malloc((varstore.vars[idx].strlen+1) * sizeof(char));
					        sprintf(strprint, "%.*s", varstore.vars[idx].strlen, varstore.vars[idx].strbeg);
				        	// Print the string
				            printf("%s", replace(strprint, "\\n", "\n"));
			                
			                return;
			            }
			        }
			    }
        	}
		    
            printf("%d\n", eval_expr(prnt->children[1]));
        }
		// If there are 4 children, it means it's a print statement with an expression
    } else if (prnt->nchildren == 4) {
        const struct node *const strl = prnt->children[1];

        // Extract the string from the token
        const uint8_t *const beg = strl->token->beg + 1;
        const uint8_t *const end = strl->token->end - 1;
        const ptrdiff_t len = end - beg;
		
		if(prnt->children[2]->children[0]->children[0]->token->tk==TK_NAME){
        	const uint8_t *const bbeg = prnt->children[2]->children[0]->children[0]->token->beg;
        	const ptrdiff_t blen = prnt->children[2]->children[0]->children[0]->token->end - bbeg;
	        for (size_t idx = 0; idx < varstore.size; ++idx) {
			    if (varstore.vars[idx].len == blen && !memcmp(varstore.vars[idx].beg, bbeg, blen)) {
			        if (varstore.vars[idx].array_size && varstore.vars[idx].type == VAR_STR) {
			            //printf("%.*s%.*s\n", (int) len, beg, varstore.vars[idx].strlen, varstore.vars[idx].strbeg);
			            
			            char *str_primary = (char *)malloc((len+1) * sizeof(char));
					    sprintf(str_primary, "%.*s", (int) len, beg);
				        
				        char *str_secondary = (char *)malloc((varstore.vars[idx].strlen+1) * sizeof(char));
					    sprintf(str_secondary, "%.*s", varstore.vars[idx].strlen, varstore.vars[idx].strbeg);
					    // Print the string
				        printf("%s%s", str_primary, replace(str_secondary, "\\n", "\n"));
			                
			            
			            return;
			        }
			    }
			}
        }
        char *strprint = (char *)malloc((len+1) * sizeof(char));
		sprintf(strprint, "%.*s", (int) len, beg);
		
        // Evaluate the expression and print the string with the evaluated result
        printf("%s%d\n", replace(strprint, "\\n", "\n"), eval_expr(prnt->children[2]));
    }
}


static void run_inpt(const struct node *const inpt)
{
	// If there are 3 children, it means it's a print statement without an expression
	if(inpt->children[1]->children[0]->token->tk==TK_NAME){
		const uint8_t *const beg = inpt->children[1]->children[0]->children[0]->token->beg;
		const ptrdiff_t len = inpt->children[1]->children[0]->children[0]->token->end - beg;
		for (size_t idx = 0; idx < varstore.size; ++idx) {
			if (varstore.vars[idx].len == len && !memcmp(varstore.vars[idx].beg, beg, len) && varstore.vars[idx].array_size) {
			    if (varstore.vars[idx].type == VAR_STR) {
			        char input[VAR_CAPACITY];
			        fgets(input, VAR_CAPACITY, stdin);
			        int inptCount = 0;
					for (int i = 0; input[i] != '\0'; i++) {
        				inptCount++;
    				}
    				char value[VAR_CAPACITY], temp[VAR_CAPACITY];
    				sprintf(temp, "%.*s", varstore.vars[idx].strlen, varstore.vars[idx].strbeg);
    				//value = 
    				//printf("{<%s>}", );
    				sprintf(value, "%s", replace((char*)varstore.vars[idx].strbeg, temp, input));
			        varstore.vars[idx].strbeg = (uint8_t*)value;
			        varstore.vars[idx].strlen = (ptrdiff_t)inptCount;
			        
			        return;
			    }else{
			    	
			    	scanf("%d", &varstore.vars[idx].values[0]);
			    	return;
			    }
			}
		}
	}else{
		const uint8_t *const beg = inpt->children[1]->children[0]->children[0]->token->beg;
		const ptrdiff_t len = inpt->children[1]->children[0]->children[0]->token->end - beg;
		for (size_t idx = 0; idx < varstore.size; ++idx) {
			if (varstore.vars[idx].len == len && !memcmp(varstore.vars[idx].beg, beg, len) && varstore.vars[idx].array_size) {
			    if (varstore.vars[idx].type == VAR_STR) {
			        char input[VAR_CAPACITY];
			        fgets(input, VAR_CAPACITY, stdin);
			        
			        int inptCount = 0;
					for (int i = 0; input[i] != '\0'; i++) {
        				inptCount++;
    				}
    				char value[VAR_CAPACITY], temp[VAR_CAPACITY];
    				sprintf(temp, "%.*s", varstore.vars[idx].strlen, varstore.vars[idx].strbeg);
    				//value = 
    				//printf("{<%s>}", );
    				sprintf(value, "%s", replace((char*)varstore.vars[idx].strbeg, temp, input));
			        varstore.vars[idx].strbeg = (uint8_t*)value;
			        varstore.vars[idx].strlen = (ptrdiff_t)inptCount;
			        return;
			    }
			}
		}
		
		prnt(40, "sorry 😪; this token is error (%d)", inpt->children[1]->children[0]->token->tk);
		
	}
}


static void run_ctrl(const struct node *const ctrl)
{
    switch (ctrl->children[0]->nt) {
    case NT_Func: {
    	run_func(ctrl->children[0]);
    } break;
    case NT_Cond: {
        // Conditional statement
        const struct node *const cond = ctrl->children[0];

        // Evaluate the condition expression
        if (eval_expr(cond->children[1])) {
            // Execute the statements in the true branch
            const struct node *stmt = cond->children[3];

            while (stmt->nchildren) {
                run_stmt(stmt++);
            }
        } else if (ctrl->nchildren >= 2) {
            // Check for "elif" and "else" branches
            size_t child_idx = 1;

            do {
                if (ctrl->children[child_idx]->nt == NT_Elif) {
                    // Evaluate the condition expression of the "elif" branch
                    const struct node *const elif = ctrl->children[child_idx];

                    if (eval_expr(elif->children[1])) {
                        // Execute the statements in the true branch of "elif"
                        const struct node *stmt = elif->children[3];

                        while (stmt->nchildren) {
                            run_stmt(stmt++);
                        }

                        break;
                    }
                } else {
                    // Execute the statements in the "else" branch
                    const struct node *const els = ctrl->children[child_idx];
                    const struct node *stmt = els->children[2];

                    while (stmt->nchildren) {
                        run_stmt(stmt++);
                    }
                }
            } while (++child_idx < ctrl->nchildren);
        }
    } break;

    case NT_Dowh: {
        // Do-while loop
        const struct node *const dowh = ctrl->children[0];
        const struct node *const expr = dowh->children[dowh->nchildren - 2];

        do {
            // Execute the statements in the loop body
            const struct node *stmt = dowh->children[2];

            while (stmt->nchildren) {
                run_stmt(stmt++);
            }
        } while (eval_expr(expr));
    } break;

    case NT_Whil: {
        // While loop
        const struct node *const whil = ctrl->children[0];

        while (eval_expr(whil->children[1])) {
            // Execute the statements in the loop body
            const struct node *stmt = whil->children[3];

            while (stmt->nchildren) {
                run_stmt(stmt++);
            }
        }
    } break;

    default:
        abort();
    }
}

static int eval_atom(const struct node *const atom)
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
	                	return varstore.vars[idx].values[0];
	                } else {
	                    return 0;
	                }
	            }
	        }
	
	        return fprintf(stderr, "warn: access to undefined variable\n"), 0;
	    }
	    case TK_NMBR: {
	        // Numeric value
	        const uint8_t *const beg = atom->children[0]->token->beg;
	        const uint8_t *const end = atom->children[0]->token->end;
	        
	        //int result = 0, mult = 1;
	        
	        /*char *strfloat = (char *)malloc((end - beg + 1) * sizeof(char));
	        sprintf(strfloat, "%.*s", end-beg, beg);
			prnt(50, "%f", stringToFloat(strfloat));*/
	        // Convert the numeric value from string to integer
	        /*for (ssize_t idx = end - beg - 1; idx >= 0; --idx, mult *= 10) {
	            result += mult * (beg[idx] - '0');
	        }*/
	        char *StrNum = (char *)malloc((end - beg + 1) * sizeof(char));
	        sprintf(StrNum, "%.*s", end-beg, beg);
	        
	        //return StrNum;
			if(contains(StrNum, ".")){
				return 0;
			}else{
				return stringToInt(StrNum);
			}
			
	        //return result;
	    }

	    default: {
	        abort();
	    }
	}
}

static int eval_expr(const struct node *const expr)
{
	switch (expr->children[0]->nt) {
	    case NT_Atom:
	        return eval_atom(expr->children[0]);
	
	    case NT_Pexp:
	        return eval_pexp(expr->children[0]);
	
	    case NT_Bexp:
	        return eval_bexp(expr->children[0]);
	
	    case NT_Uexp:
	        return eval_uexp(expr->children[0]);
	
	    case NT_Texp:
	        return eval_texp(expr->children[0]);
	
	    case NT_Aexp:
	        return eval_aexp(expr->children[0]);
	
	    default:
	        abort();
    }
}

static int eval_pexp(const struct node *const pexp)
{
    // Parenthesized expression, evaluate the expression inside
    return eval_expr(pexp->children[1]);
}

static int eval_bexp(const struct node *const bexp)
{
    switch (bexp->children[1]->token->tk) {
    case TK_PLUS:
        // Addition
        return eval_expr(bexp->children[0]) + eval_expr(bexp->children[2]);

    case TK_MINS:
        // Subtraction
        return eval_expr(bexp->children[0]) - eval_expr(bexp->children[2]);

    case TK_MULT:
        // Multiplication
        return eval_expr(bexp->children[0]) * eval_expr(bexp->children[2]);

    case TK_DIVI: {
        // Division
        const int dividend = eval_expr(bexp->children[0]);
        const int divisor = eval_expr(bexp->children[2]);

        if (divisor) {
            return dividend / divisor;
        } else {
            fprintf(stderr, "warn: prevented attempt to divide by zero\n");
            return 0;
        }
    }

    case TK_MODU:
        // Modulo
        return eval_expr(bexp->children[0]) % eval_expr(bexp->children[2]);

    case TK_EQUL:
        // Equality
        return eval_expr(bexp->children[0]) == eval_expr(bexp->children[2]);

    case TK_NEQL:
        // Inequality
        return eval_expr(bexp->children[0]) != eval_expr(bexp->children[2]);

    case TK_LTHN:
        // Less than
        return eval_expr(bexp->children[0]) < eval_expr(bexp->children[2]);

    case TK_GTHN:
        // Greater than
        return eval_expr(bexp->children[0]) > eval_expr(bexp->children[2]);

    case TK_LTEQ:
        // Less than or equal to
        return eval_expr(bexp->children[0]) <= eval_expr(bexp->children[2]);

    case TK_GTEQ:
        // Greater than or equal to
        return eval_expr(bexp->children[0]) >= eval_expr(bexp->children[2]);

    case TK_CONJ:
        // Logical conjunction (AND)
        return eval_expr(bexp->children[0]) && eval_expr(bexp->children[2]);

    case TK_DISJ:
        // Logical disjunction (OR)
        return eval_expr(bexp->children[0]) || eval_expr(bexp->children[2]);

    default:
        abort();
    }
}

static int eval_uexp(const struct node *const uexp)
{
    // Evaluate unary expressions
    switch (uexp->children[0]->token->tk) {
    case TK_PLUS:
        // Unary plus
        return eval_expr(uexp->children[1]);

    case TK_MINS:
        // Unary minus
        return -eval_expr(uexp->children[1]);

    case TK_NEGA:
        // Logical negation
        return !eval_expr(uexp->children[1]);

    default:
        abort();
    }
}

static int eval_texp(const struct node *const texp)
{
    // Evaluate ternary expressions
    return eval_expr(texp->children[0]) ? eval_expr(texp->children[2]) : eval_expr(texp->children[4]);
}

static int eval_aexp(const struct node *const aexp)
{
    // Evaluate array expressions
    const uint8_t *const beg = aexp->children[0]->token->beg;
    const ptrdiff_t len = aexp->children[0]->token->end - beg;
    const int array_idx = eval_expr(aexp->children[2]);

    // Check if the array index is negative
    if (array_idx < 0) {
        return fprintf(stderr, "warn: negative array offset\n"), 0;
    }

    // Search for the array in the varstore
    for (size_t idx = 0; idx < varstore.size; ++idx) {
        if (varstore.vars[idx].len == len && !memcmp(varstore.vars[idx].beg, beg, len)) {
            // Check if the array index is within the bounds
            if (array_idx < varstore.vars[idx].array_size) {
                return varstore.vars[idx].values[array_idx];
            } else {
                return fprintf(stderr, "warn: out of bounds array access\n"), 0;
            }
        }
    }

    return fprintf(stderr, "warn: access to undefined array\n"), 0;
}
