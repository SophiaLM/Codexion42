#ifndef CODEXION_H
# define CODEXION_H

# include <stdlib.h>
# include <unistd.h>

typedef enum e_error
{
	ERR_OK = 0,
	ERR_ARGC,
	ERR_NOT_NUMERIC,
	ERR_OVERFLOW,
	ERR_ZERO,
	ERR_NEGATIVE,
	ERR_SCHEDULER,
}	t_error;

int		ft_atoi(const char *str);
void	ft_bzero(void *s, size_t n);
void	*ft_calloc(size_t nmemb, size_t size);
int		ft_validnumber(const char *str);
void	*ft_memset(void *dest, int c, size_t count);
void	ft_putchar(char c);
void	ft_putendl(const char *str);
void	ft_putstr(const char *str);
size_t	ft_strlen(const char *str);
int		ft_strncmp(const char *s1, const char *s2, size_t n);

t_error	ft_check_overflow(const char *str);
t_error	ft_is_valid_positive(const char *str);
t_error	ft_is_valid_non_negative(const char *str);
t_error	ft_is_valid_scheduler(const char *str);
void	print_error(t_error code);
int		parse_args(int argc, char **argv);

#endif
