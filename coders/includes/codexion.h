#ifndef CODEXION_H
# define CODEXION_H

# include <stdlib.h>
# include <unistd.h>
# include <limits.h>

/* Errores */

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

/* scheduler */

typedef enum e_scheduler
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF,
}	t_scheduler;

/* Los 8 argumentos validados */

typedef struct s_config
{
	int			number_of_coders;
	long long	time_to_burnout;
	long long	time_to_compile;
	long long	time_to_debug;
	long long	time_to_refactor;
	int			number_of_compiles_required;
	long long	dongle_cooldown;
	t_scheduler	scheduler;
}	t_config;

/* mutex, variable de condición, estado (libre/ocupado), * 
 * timestamp `available_at` (para el cooldown), heap de peticiones*/

typedef struct s_dongle
{
	int			id;
	int			is_taken;
	long long	available_at_ms;
}	t_dongle;

/* id, handle del hilo, `last_compile_start`, `compile_count` */

typedef struct s_coder
{
	int			id;
	long long	last_compile_start;
	int			compile_count;
	t_dongle	*first;
	t_dongle	*second;
	struct s_sim	*sim;
}	t_coder;


/* configuración, hora de inicio, array de coders, array de dongles, *
 *  flag de parada + su mutex, mutex de impresión, hilo monitor. */

typedef struct s_sim
{
	t_config	config;
	long long	start_time_ms;
	t_coder		*coders;
	t_dongle	*dongles;
	int			stop;
}	t_sim;

int		ft_atoi(const char *str, int *out);
int		ft_atoll(const char *str, long long *out);
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
int		parse_args(int argc, char **argv, t_config *cfg);

#endif
