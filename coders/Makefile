NAME = codexion

CFLAGS = -Wall -Wextra -Werror -pthread

CC = cc

RM = rm -f

PASTEL_PINK = \033[38;5;218m

STRONG_PINK = \033[38;5;204m

LILAC = \033[38;5;141m

RESET = \033[0m

HEADER = includes/codexion.h

SRC = srcs/auxiliary_libft/ft_atoi.c \
      srcs/auxiliary_libft/ft_bzero.c \
      srcs/auxiliary_libft/ft_calloc.c \
      srcs/auxiliary_libft/ft_validnumber.c \
      srcs/auxiliary_libft/ft_memset.c \
      srcs/auxiliary_libft/ft_putchar.c \
      srcs/auxiliary_libft/ft_putendl.c \
      srcs/auxiliary_libft/ft_putstr.c \
      srcs/auxiliary_libft/ft_strlen.c \
      srcs/auxiliary_libft/ft_strncmp.c \
      srcs/parcing/parcing.c \
      srcs/parcing/parching_utils.c \
      srcs/codexion.c

OBJ = $(SRC:.c=.o)

all: $(NAME) kitty

$(NAME): $(OBJ)
	@echo "$(PASTEL_PINK)creating $(STRONG_PINK)$(NAME)..."
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

srcs/auxiliary_libft/%.o: srcs/auxiliary_libft/%.c $(HEADER)
	@echo "$(PASTEL_PINK)compiling $(STRONG_PINK)$<..."
	@$(CC) $(CFLAGS) -c $< -o $@

srcs/parcing/%.o: srcs/parcing/%.c $(HEADER)
	@echo "$(PASTEL_PINK)compiling $(STRONG_PINK)$<..."
	@$(CC) $(CFLAGS) -c $< -o $@

srcs/%.o: srcs/%.c $(HEADER)
	@echo "$(PASTEL_PINK)compiling $(STRONG_PINK)$<..."
	@$(CC) $(CFLAGS) -c $< -o $@

kitty:
	@echo "$(STRONG_PINK)  /\_/\                       /\_/\ $(RESET)"
	@echo "$(STRONG_PINK) ( ovo ) $(PASTEL_PINK)¡codexion is ready! $(STRONG_PINK)( owo )$(RESET)"
	@echo "$(STRONG_PINK)   >🐟 <                     > 🐟 < $(RESET)"

clean: clean_kitty
	@$(RM) $(OBJ)

clean_kitty:
	@echo "$(STRONG_PINK)  /\_/\                          /\_/\ $(RESET)"
	@echo "$(STRONG_PINK) ( -.- ) $(PASTEL_PINK)¡All objects removed!  $(STRONG_PINK)( -w- )$(RESET)"
	@echo "$(STRONG_PINK)   >🗑️ <                        > 🗑️ < $(RESET)"

fclean: fclean_kitty
	@$(MAKE) clean > /dev/null 2>&1
	@$(RM) $(NAME)

fclean_kitty:
	@echo "$(STRONG_PINK)  /\_/\                             /\_/\ $(RESET)"
	@echo "$(STRONG_PINK) ( >n< ) $(PASTEL_PINK)¡Full cleanup completed!  $(STRONG_PINK)( >w< )$(RESET)"
	@echo "$(STRONG_PINK)  > 💀 <                           > 💀 < $(RESET)"

re: fclean all

.PHONY: all clean fclean re kitty clean_kitty fclean_kitty
