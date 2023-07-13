#include "include/lexer.h"
#include "include/visitor_expr.h"
#include "include/utils.h"
#include "include/parser.h"
#include "include/visitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

static void visit_stmt(const struct node *const);     // Function to execute a statement node
static void visit_assn(const struct node *const);     // Function to execute an assignment node
static void visit_func(const struct node *const);
static void visit_prnt(const struct node *const, char *);
//static void visit_inpt(const struct node *const);     // Function to execute a inpt node
static void visit_ctrl(const struct node *const);     // Function to execute a control node

enum {
	VAR_INT,			//Variable type integer
	VAR_STR,			//Variable type string
	VAR_FLT,			//Variable type float
	VAR_BOL,			//Variable type bool
};

void visitor(const struct node *const unit)
{
    // Execute each statement in the program
    for (size_t stmt_idx = 1; stmt_idx < unit->nchildren - 1; ++stmt_idx) {
        visit_stmt(unit->children[stmt_idx]);
    }
    // Free the memory associated with variable values
    for (size_t var_idx = 0; var_idx < varstore.size; ++var_idx) {
        free(varstore.vars[var_idx].values);
    }
    // Reset the size of the variable store
    varstore.size = 0;
}

static void visit_stmt(const struct node *const stmt)
{
    switch (stmt->children[0]->nt) {
        case NT_Assn:                                  // If the statement is an assignment statement
            visit_assn(stmt->children[0]);               // Execute the visit_assn function for the assignment
            break;
        case NT_Ctrl:                                  // If the statement is a control statement
            visit_ctrl(stmt->children[0]);               // Execute the visit_ctrl function for the control statement
            break;
        default:
            abort();                                   // If the statement is of an unknown type, abort the program
    }
}

static void visit_assn(const struct node *const assn)
{
	
    // Check if the left-hand side of the assignment is an array expression
    const int lhs_is_aexp = assn->children[0]->nchildren;
	
	// Evaluate the array index if present, or set it to 0
    const int array_idx = lhs_is_aexp ? get_int_expr(assn->children[0]->children[2]) : 0;
	
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
            uint8_t datatype = get_type_expr(assn->children[2]);
			
            // Check if the array index is within the bounds
            if (array_idx >= 0 && array_idx < array_size) {
				if(datatype == VAR_INT){
					varstore.vars[var_idx].type = VAR_INT;
					varstore.vars[var_idx].string = NULL;
		        	// Evaluate the expression on the right-hand side and assign the value
		        	varstore.vars[var_idx].values[array_idx] = get_int_expr(assn->children[2]);
				} else if(datatype == VAR_STR){
					// Evaluate the expression on the right-hand side and assign the value
		        	varstore.vars[var_idx].string = get_str_expr(assn->children[2]);;
		        	varstore.vars[var_idx].type = VAR_STR;
		        	varstore.vars[var_idx].values[array_idx] = 0;
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
				
				uint8_t datatype = get_type_expr(assn->children[2]);
		
				if(datatype == VAR_INT){
					varstore.vars[var_idx].type = VAR_INT;
					varstore.vars[var_idx].string = NULL;
		        	// Evaluate the expression on the right-hand side and assign the value
		        	varstore.vars[var_idx].values[array_idx] = get_int_expr(assn->children[2]);
				} else if(datatype == VAR_STR){
					// Evaluate the expression on the right-hand side and assign the value
		        	varstore.vars[var_idx].string = get_str_expr(assn->children[2]);;
		        	varstore.vars[var_idx].type = VAR_STR;
		        	varstore.vars[var_idx].values[array_idx] = 0;
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
		
		
		if (!varstore.vars[var_idx].values) {
            perror("malloc");
            return;
        }
		
        varstore.vars[var_idx].array_size = array_idx + 1;
		
		uint8_t datatype = get_type_expr(assn->children[2]);
		
		if(datatype == VAR_INT){
			varstore.vars[var_idx].type = VAR_INT;
			varstore.vars[var_idx].string = NULL;
        	// Evaluate the expression on the right-hand side and assign the value
        	varstore.vars[var_idx].values[array_idx] = get_int_expr(assn->children[2]);
		} else if(datatype == VAR_STR){
			// Evaluate the expression on the right-hand side and assign the value
        	varstore.vars[var_idx].string = get_str_expr(assn->children[2]);;
        	varstore.vars[var_idx].type = VAR_STR;
        	varstore.vars[var_idx].values[array_idx] = 0;
		}else if(datatype == VAR_FLT){
			// Evaluate the expression on the right-hand side and assign the value
        	varstore.vars[var_idx].Float = get_flt_expr(assn->children[2]);;
        	varstore.vars[var_idx].type = VAR_FLT;
        	varstore.vars[var_idx].values[array_idx] = 0;
		}
		
		varstore.size++;
    } else {
        fprintf(stderr, "warn: varstore exhausted, assignment has no effect\n");
    }
}

static void visit_func(const struct node *const func)
{
	if(func->children[0]->token->tk == TK_FUNC && func->children[1]->token->tk == TK_NAME)
	{
		int pramloop = 0;
		uint8_t pramtk1, pramtk2;
			
		while(1){
			pramloop++;
			pramtk1 = func->children[2+pramloop]->token->tk;
			pramtk2 = func->children[3+pramloop]->token->tk;
			if(pramtk1 == 10 && pramtk2 == 13){
				pramloop--;
				break;
			}
			
		}
		
		const uint8_t *beg = func->children[1]->token->beg;
		const ptrdiff_t len = func->children[1]->token->end - func->children[1]->token->beg;
		
		struct node *stmt = func->children[5 + (pramloop)];
		
		size_t fun_idx;
		for (fun_idx = 0; fun_idx < function.size; ++fun_idx) {
			
		}
		
		if(fun_idx < VAR_CAPACITY){
			function.get[fun_idx].beg = beg;
			function.get[fun_idx].len = len;
			function.get[fun_idx].children = stmt;			
		}
		
		function.size++;
	}else if((func->children[0]->token->tk == TK_NAME) && 
			((func->children[1]->token->tk == TK_LPAR) ||
			(func->children[1]->token->tk == TK_STRL && 
			func->children[2]->token->tk == TK_SCOL)))
	{

		const uint8_t *beg = func->children[0]->token->beg;
		const ptrdiff_t len = func->children[0]->token->end - func->children[0]->token->beg;
		
		char *FuncName = (char *)malloc((len+1) * sizeof(char));
	    sprintf(FuncName, "%.*s", len, beg);
        
		if(contains(FuncName, "prnt") || contains(FuncName, "print")){
			visit_prnt(func, FuncName);
			return;
		}
		for (size_t idx = 0; idx < function.size; ++idx) {
	        if (function.get[idx].len == len && !memcmp(function.get[idx].beg, beg, len)) {
	            while (function.get[idx].children->nchildren) {
                	visit_stmt(function.get[idx].children++);
                }
	            return;
	        }
	    }
	    fprintf(stderr, "FunctionError: name '%.*s' is not defined\n", len, beg);
	}
}

static void visit_prnt(const struct node *const func, char *FuncName)
{
	if(func->nchildren >= 4){
		int totalparam = func->nchildren - 4;
		for(int i = 0; i < totalparam; i++){
			if(i != totalparam-1){
				uint8_t datatype = get_type_expr(func->children[i+2]->children[0]->children[0]);
				if(datatype == VAR_INT){
					printf("%d", get_int_expr(func->children[i+2]->children[0]->children[0]));
				} else if(datatype == VAR_STR){
					printf("%s", get_str_expr(func->children[i+2]->children[0]->children[0]));
				}else if(datatype == VAR_FLT){
					printf("%f", get_flt_expr(func->children[i+2]->children[0]->children[0]));
				}
			}else{
				uint8_t datatype = get_type_expr(func->children[i+2]);
				if(datatype == VAR_INT){
					printf("%d", get_int_expr(func->children[i+2]));
				} else if(datatype == VAR_STR){
					printf("%s", get_str_expr(func->children[i+2]));
				}else if(datatype == VAR_FLT){
					printf("%f", get_flt_expr(func->children[i+2]));
				}
			}
		}
		return;
	}else if(func->nchildren == 3 && func->children[1]->token->tk == TK_STRL){
		const uint8_t *const beg = func->children[1]->token->beg + 1;
        const ptrdiff_t len = (func->children[1]->token->end - 1) - beg;
        
        char *getSTRL = (char *)malloc((len) * sizeof(char));
	    sprintf(getSTRL, "%.*s", len, beg);
        ScapeSequence(&getSTRL);
		
		printf("%s", getSTRL);
	}
}

static void visit_ctrl(const struct node *const ctrl)
{
    switch (ctrl->children[0]->nt) {
    case NT_Func: {
    	visit_func(ctrl->children[0]);
    } break;
    case NT_Cond: {
        // Conditional statement
        const struct node *const cond = ctrl->children[0];

        // Evaluate the condition expression
        if (get_bol_expr(cond->children[1])) {
            // Execute the statements in the true branch
            const struct node *stmt = cond->children[3];

            while (stmt->nchildren) {
                visit_stmt(stmt++);
            }
        } else if (ctrl->nchildren >= 2) {
            // Check for "elif" and "else" branches
            size_t child_idx = 1;

            do {
                if (ctrl->children[child_idx]->nt == NT_Elif) {
                    // Evaluate the condition expression of the "elif" branch
                    const struct node *const elif = ctrl->children[child_idx];

                    if (get_bol_expr(elif->children[1])) {
                        // Execute the statements in the true branch of "elif"
                        const struct node *stmt = elif->children[3];

                        while (stmt->nchildren) {
                            visit_stmt(stmt++);
                        }

                        break;
                    }
                } else {
                    // Execute the statements in the "else" branch
                    const struct node *const els = ctrl->children[child_idx];
                    const struct node *stmt = els->children[2];

                    while (stmt->nchildren) {
                        visit_stmt(stmt++);
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
                visit_stmt(stmt++);
            }
        } while (get_bol_expr(expr));
    } break;

    case NT_Whil: {
        // While loop
        const struct node *const whil = ctrl->children[0];

        while (get_bol_expr(whil->children[1])) {
            // Execute the statements in the loop body
            const struct node *stmt = whil->children[3];

            while (stmt->nchildren) {
                visit_stmt(stmt++);
            }
        }
    } break;

    default:
        abort();
    }
}
