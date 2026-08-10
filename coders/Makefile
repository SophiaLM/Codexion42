NAME = codexion

CFLAGS = -Wall -Wextra -Werror -pthread

CC = cc

RM = rm -f

PASTEL_PINK = \033[38;5;218m

STRONG_PINK = \033[38;5;204m

LILAC = \033[38;5;141m

RESET = \033[0m

HEADER = includes/codexion.h

SRC = src/auxiliary_libft/ft_atoi.c \
      src/auxiliary_libft/ft_atoll.c \
      src/auxiliary_libft/ft_bzero.c \
      src/auxiliary_libft/ft_calloc.c \
      src/auxiliary_libft/ft_validnumber.c \
      src/auxiliary_libft/ft_putchar.c \
      src/auxiliary_libft/ft_putchar_fd.c \
      src/auxiliary_libft/ft_putendl.c \
      src/auxiliary_libft/ft_putendl_fd.c \
      src/auxiliary_libft/ft_putstr.c \
      src/auxiliary_libft/ft_putstr_fd.c \
      src/auxiliary_libft/ft_putnbr.c \
      src/auxiliary_libft/ft_strlen.c \
      src/auxiliary_libft/ft_strncmp.c \
      src/parcing/parcing.c \
      src/parcing/parching_utils.c \
      src/queue/queue_init.c \
      src/queue/queue_utils.c \
      src/queue/queue_push.c \
      src/queue/queue_pop.c \
      src/time_logging/time_utils.c \
      src/time_logging/smart_sleep.c \
      src/time_logging/sim_stopped.c \
      src/time_logging/log_state.c \
      src/coder_routine/coder_routine.c \
      src/coder_routine/dongles_fake.c \
      src/coder_routine/set_compile_start.c \
      src/sim/sim_init.c \
      src/sim/sim_destroy.c \
      src/sim/sim_cleanup.c \
      src/codexion.c

OBJ = $(SRC:.c=.o)

all: $(NAME) kitty

$(NAME): $(OBJ)
	@echo "$(PASTEL_PINK)creating $(STRONG_PINK)$(NAME)..."
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

src/auxiliary_libft/%.o: src/auxiliary_libft/%.c $(HEADER)
	@echo "$(PASTEL_PINK)compiling $(STRONG_PINK)$<..."
	@$(CC) $(CFLAGS) -c $< -o $@

src/parcing/%.o: src/parcing/%.c $(HEADER)
	@echo "$(PASTEL_PINK)compiling $(STRONG_PINK)$<..."
	@$(CC) $(CFLAGS) -c $< -o $@

src/time_logging/%.o: src/time_logging/%.c $(HEADER)
	@echo "$(PASTEL_PINK)compiling $(STRONG_PINK)$<..."
	@$(CC) $(CFLAGS) -c $< -o $@

src/coder_routine/%.o: src/coder_routine/%.c $(HEADER)
	@echo "$(PASTEL_PINK)compiling $(STRONG_PINK)$<..."
	@$(CC) $(CFLAGS) -c $< -o $@

src/sim/%.o: src/sim/%.c $(HEADER)
	@echo "$(PASTEL_PINK)compiling $(STRONG_PINK)$<..."
	@$(CC) $(CFLAGS) -c $< -o $@

src/queue/%.o: src/queue/%.c $(HEADER)
	@echo "$(PASTEL_PINK)compiling $(STRONG_PINK)$<..."
	@$(CC) $(CFLAGS) -c $< -o $@

src/%.o: src/%.c $(HEADER)
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
