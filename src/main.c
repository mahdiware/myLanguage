#include "include/main.h"
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

//if showMe is 1 then it is defined and displayed to the lexer and interpreter when running the code.
#if defined(SHOW_ME)
	#define showMe 1
#else
	#define showMe 0
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
    printf("  --version          show version information and exit\n");
    printf("  --help             show command line usage and exit\n");
    printf("  -ms                print millitime\n");
    
}

static void print_version (void) {
	printf("Version: 1.1\n");
}
static action_type parse_args(int argc, char **argv) {
    if (argc == 1) return REPL;

    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        print_version();
        exit(0);
    }

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        print_help();
        exit(0);
    }

    action_type type = NONE;
    for (int i=1; i<argc; ++i) {
        if ((strcmp(argv[i], "-ms") == 0) && (i+1 < argc)) {
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
    int fd;
    size_t size;
    struct stat statbuf;
    int exit_status = EXIT_FAILURE;

    if ((fd = open(input_file, O_RDONLY)) < 0) {
        return perror("open"), exit_status;
    }

    if (fstat(fd, &statbuf) < 0) {
        return perror("fstat"), close(fd), exit_status;
    }

    if ((size = statbuf.st_size) == 0) {
        fprintf(stderr, "‘%s‘: file is empty\n", input_file);
        return close(fd), exit_status;
    }

    const uint8_t *const mapped = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (mapped == MAP_FAILED) {
        return perror("mmap"), close(fd), exit_status;
    }
	
    struct token *tokens;
    size_t ntokens;
    
    
    const int lex_error = lexer(mapped, size, &tokens, &ntokens);
	if ((!lex_error || lex_error == LEX_UNKNOWN_TOKEN) && showMe) {
    	puts(WHITE("*** Lexing ***"));
        print_tokens(tokens, ntokens, lex_error);
    } else if (lex_error == LEX_NOMEM) {
        puts(RED("The lexer could not allocate memory."));
    }

    if (!lex_error) {
    	if(showMe) puts(WHITE("\n*** Parsing ***"));
        const struct node root = parser(tokens, ntokens);

        if (!parser_error(root)) {
        	if(showMe) puts(WHITE("\n*** Running ***"));
            visitor(&root);
            destroy_tree(root);
            exit_status = EXIT_SUCCESS;
        }
    }

    free(tokens);
    munmap((uint8_t *const) mapped, size);
    close(fd);
    return exit_status;
}

int main(int argc, char **argv)
{
	action_type type = parse_args(argc, argv);
	uint64_t tstart = nanotime();
	if (type == NONE || type == MSEC) {
		start(argc, argv);
    }else if(type == REPL){
    	return fprintf(stderr, "Usage: %s <file>\n", argv[0]), EXIT_FAILURE;
    }
    if(type == MSEC){
    	uint64_t tend = nanotime();
    	printf("\nTime: %.4f ms\n", millitime(tstart, tend));
    }
}