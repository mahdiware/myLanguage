CFLAGS = -std=gnu11 -DLANG_READLINE -Wall -Werror
NAME = lang

SRCDIR := ./src
OBJDIR := ./obj
SRCS := $(addprefix $(SRCDIR)/, $(wildcard $(SRCDIR)/*.c))
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))

all: $(NAME)

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $^

clear: c
run: r
c:
	rm -rf $(OBJDIR)
	rm -f $(NAME)
r:
	@make
	@cp -r $(NAME) /usr/local/bin/
	@chmod +x /usr/local/bin/$(NAME)
