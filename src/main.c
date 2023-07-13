#include "include/main.h"
#include "include/builtin.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/visitor.h"
#include "include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#if defined(LANG_READLINE)
  #include <readline/readline.h>
  #include <readline/history.h>
  #include <readline/tilde.h>
#endif

static const char *input_file = NULL;


static void print_tokens(const struct token *const tokens, const size_t ntokens, const int error) 
{
    for (size_t i = 0, alternate = 0; i < ntokens; ++i) {
        const struct token token = tokens[i];

        if (token.tk == TK_FBEG || token.tk == TK_FEND) {
            continue;
        }

        if (token.tk != TK_WSPC && token.tk != TK_LCOM && token.tk != TK_BCOM) {
            alternate++;
        }

        const int len = token.end - token.beg;
        
        if (i == ntokens - 1 && error == LEX_UNKNOWN_TOKEN) {
            printf(RED("%.*s") CYAN("< Unknown token\n"), len ?: 1, token.beg);
        } else if (token.tk == TK_LCOM || token.tk == TK_BCOM) {
            printf(GRAY("%.*s"), len, token.beg);
        } else if(token.tk == TK_NAME){
        	printf(CYAN("%.*s"), len, token.beg);
        } else if (alternate % 2) {
            printf(GREEN("%.*s"), len, token.beg);
        } else {
            printf(YELLOW("%.*s"), len, token.beg);
        }
    }
}

static void print_help (void) {
    printf("Usage: lang [options] [arguments...]\n");
    printf("\n");
    printf("To start the REPL (not yet supported):\n");
    printf("  lang\n");
    printf("\n");
    printf("To compile and execute file:\n");
    printf("  lang example.mw\n");
    printf("\n");
    printf("Available options are:\n");
    printf("  -v,  --version          show version information and exit\n");
    printf("  -h,  --help             show command line usage and exit\n");
    printf("  -ms                	  print millitime\n");
    
}

static void print_version (void) {
	printf("%s\n", COPYRIGHT);
}
static action_type parse_args(int argc, char **argv) {
    if (argc == 1) return REPL;

    if (argc == 2 && (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version"))) {
        print_version();
        exit(0);
    }

    if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
        print_help();
        exit(0);
    }

    action_type type = NONE;
    for (int i=1; i<argc; ++i) {
        if ((!strcmp(argv[i], "-ms")) && (i+1 < argc)) {
        	input_file = argv[++i];
            type = MSEC;
        }else{
        	input_file = argv[i];
        }
    }

    return type;
}

int start(int argc, char **argv)
{
    size_t size;
    int exit_status = EXIT_FAILURE;
	
	const uint8_t *const source_code = file_reader(input_file, &size, &exit_status);
	
	if(exit_status == EXIT_FAILURE)
		return EXIT_FAILURE;
	
    struct token *tokens;
    size_t ntokens;
    
    
    const int lex_error = lexer(source_code, size, &tokens, &ntokens);
	if ((!lex_error || lex_error == LEX_UNKNOWN_TOKEN)) {
		#if defined(SHOW_ME)
    		puts(WHITE("*** Lexing ***"));
    		print_tokens(tokens, ntokens, lex_error);
    	#endif
    } else if (lex_error == LEX_NOMEM) {
        puts(RED("The lexer could not allocate memory."));
    }

    if (!lex_error) {
    	#if defined(SHOW_ME)
    		puts(WHITE("\n*** Parsing ***"));
    	#endif
        const struct node root = parser(tokens, ntokens);

        if (!parser_error(root)) {
        	#if defined(SHOW_ME)
        		puts(WHITE("\n*** Running ***"));
        	#endif
            visitor(&root);
            destroy_tree(root);
            exit_status = EXIT_SUCCESS;
        }
    }

    free(tokens);
    munmap((uint8_t *const) source_code, size);
    return exit_status;
}

int main(int argc, char **argv)
{
	int exit_status = EXIT_FAILURE;
	
	action_type type = parse_args(argc, argv);
	uint64_t tstart = nanotime();
	
	if (type == NONE || type == MSEC) {
		exit_status = start(argc, argv);
    }else if(type == REPL){
    	return fprintf(stderr, "Usage: %s <file>\n", argv[0]), EXIT_FAILURE;
    }
    if(type == MSEC){
    	uint64_t tend = nanotime();
    	printf("\nTime: %.4f ms\n", millitime(tstart, tend));
    }
    
    return exit_status;
}