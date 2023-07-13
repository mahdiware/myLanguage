#-Wall -Werror
CFLAGS = -std=gnu11
NAME = lang
CC ?= gcc

MAIN := ./src/main/
LEXER := ./src/lexer/
PARSER := ./src/parser/
VISITOR := ./src/visitor/
UTILITY := ./src/utility/

SRCDIR := ./src
OBJDIR := ./obj
SRCS := $(addprefix $(SRCDIR)/, $(wildcard $(SRCDIR)/*.c))
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))

ifeq ($(OS),Windows_NT)
	# Windows
	LIBTARGET = $(NAME).dll
	LDFLAGS = -lm -lShlwapi
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		# MacOS
		LIBTARGET = lib$(NAME).dylib
		LDFLAGS = -lm
	else ifeq ($(UNAME_S),OpenBSD)
		# OpenBSD
		LIBTARGET = lib$(NAME).so
		LDFLAGS = -lm
	else ifeq ($(UNAME_S),FreeBSD)
		# FreeBSD
		LIBTARGET = lib$(NAME).so
		LDFLAGS = -lm
	else ifeq ($(UNAME_S),NetBSD)
		# NetBSD
		LIBTARGET = lib$(NAME).so
		LDFLAGS = -lm
	else ifeq ($(UNAME_S),DragonFly)
		# DragonFly
		LIBTARGET = lib$(NAME).so
		LDFLAGS = -lm
	else
		# Linux
		LIBTARGET = lib$(NAME).so
		LDFLAGS = -lm -lrt
	endif
endif

all: $(NAME)

ShowMe:
	$(MAKE) $(ALL) CFLAGS="-std=gnu11 -DSHOW_ME"	

ShowMe-Linux:
	@$(MAKE) $(ALL) CFLAGS="-std=gnu11 -fgnu89-inline -fPIC -DLANG_READLINE -DSHOW_ME"	
	@cp -r $(NAME) /usr/local/bin/
	@chmod +x /usr/local/bin/$(NAME)

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $^

lib: $(NAME)
	$(CC) -shared -o $(LIBTARGET) $(OBJS) $(LDFLAGS)

c: clean
i: install

clean:
	rm -rf $(OBJDIR)
	rm -f $(NAME)
install:
	@make
	@cp -r $(NAME) /usr/local/bin/
	@chmod +x /usr/local/bin/$(NAME)
