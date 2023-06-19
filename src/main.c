#include "lex.h"
#include "parse.h"
#include "run.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#if defined(LANG_READLINE)	/* { */

#include <readline/readline.h>
#include <readline/history.h>
#define lang_initreadline(L)	((void)L, rl_readline_name="lang")
#define lang_readline(L,b,p)	((void)L, ((b)=readline(p)) != NULL)
#define lang_saveline(L,line)	((void)L, add_history(line))
#define lang_freeline(L,b)	((void)L, free(b))

#else				/* }{ */

#define lang_initreadline(L)  ((void)L)
#define lang_readline(L,b,p) \
        ((void)L, fputs(p, stdout), fflush(stdout),  /* show prompt */ \
        fgets(b, LANG_MAXINPUT, stdin) != NULL)  /* get line */
#define lang_saveline(L,line)	{ (void)L; (void)line; }
#define lang_freeline(L,b)	{ (void)L; (void)b; }

#endif				/* } */



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

int running(int argc, char **argv)
{
    int fd;
    size_t size;
    struct stat statbuf;
    int exit_status = EXIT_FAILURE;

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        return perror("open"), exit_status;
    }

    if (fstat(fd, &statbuf) < 0) {
        return perror("fstat"), close(fd), exit_status;
    }

    if ((size = statbuf.st_size) == 0) {
        fprintf(stderr, "‘%s‘: file is empty\n", argv[1]);
        return close(fd), exit_status;
    }

    const uint8_t *const mapped = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (mapped == MAP_FAILED) {
        return perror("mmap"), close(fd), exit_status;
    }

    struct token *tokens;
    size_t ntokens;
    const int lex_error = lex(mapped, size, &tokens, &ntokens);

    if ((!lex_error || lex_error == LEX_UNKNOWN_TOKEN) && development == 1) {
    	puts(WHITE("*** Lexing ***"));
        print_tokens(tokens, ntokens, lex_error);
    } else if (lex_error == LEX_NOMEM) {
        puts(RED("The lexer could not allocate memory."));
    }

    if (!lex_error) {
    	if(development == 1) puts(WHITE("\n*** Parsing ***"));
        const struct node root = parse(tokens, ntokens);

        if (!parse_error(root)) {
        	if(development == 1) puts(WHITE("\n*** Running ***"));
            run(&root);
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
	if (argc == 2) {
		running(argc, argv);
        //return fprintf(stderr, "Usage: %s <file>\n", argv[0]), EXIT_FAILURE;
    }else{
    	char input[VAR_CAPACITY];
    	fgets(input, VAR_CAPACITY, stdin);
    	printf("%s", input);
    }
    
	return 0;
}