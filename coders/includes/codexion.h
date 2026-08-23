/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <marvin@42.fr>                    +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* e_error: error codes */
/* e_scheduler: scheduler type */
/* s_config: the 8 validated arguments */
/* s_dongle: mutex, condition variable, holder (0 = free), cooldown,
 * FIFO/EDF request queue, ticket counter */
/* s_coder: id, thread handle, state mutex, last_compile_start,
 * compile_count, first/second dongle pointers, back-pointer to sim */
/* s_sim: config, start time, coders array, dongles array,
 * stop flag + its mutex, print mutex, monitor thread.
 * NEVER hold stop_mutex when calling log_state() — see day 5 */

#ifndef CODEXION_H
# define CODEXION_H
# include <stdlib.h>
# include <unistd.h>
# include <limits.h>
# include "queue.h"
# include <pthread.h>
# include <sys/time.h>
# define SLEEP_STEP_US 200
# define BURNOUT_TOLERANCE_MS 10
# define COND_POLL_MS 10

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

typedef enum e_scheduler
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF,
}	t_scheduler;

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

typedef struct s_dongle
{
	int				id;
	int				holder;
	long long		available_at_ms;
	pthread_mutex_t	mtx;
	pthread_cond_t	cond;
	t_heap			queue;
	long long		next_ticket;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	pthread_mutex_t	state_mutex;
	long long		last_compile_start;
	int				compile_count;
	t_dongle		*first;
	t_dongle		*second;
	struct s_sim	*sim;
}	t_coder;

typedef struct s_sim
{
	t_config		config;
	long long		start_time_ms;
	t_coder			*coders;
	t_dongle		*dongles;
	int				stop;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	print_mutex;
}	t_sim;

int			ft_atoi(const char *str, int *out);
int			ft_atoll(const char *str, long long *out);
void		ft_bzero(void *s, size_t n);
void		*ft_calloc(size_t nmemb, size_t size);
int			ft_validnumber(const char *str);
void		ft_putchar(char c);
void		ft_putchar_fd(char c, int fd);
void		ft_putendl(const char *str);
void		ft_putendl_fd(const char *str, int fd);
void		ft_putstr(const char *str);
void		ft_putstr_fd(const char *str, int fd);
void		ft_putnbr(long long n);
size_t		ft_strlen(const char *str);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
t_error		ft_check_overflow(const char *str);
t_error		ft_is_valid_positive(const char *str);
t_error		ft_is_valid_non_negative(const char *str);
t_error		ft_is_valid_scheduler(const char *str);
void		print_error(t_error code);
int			parse_args(int argc, char **argv, t_config *cfg);
long long	now_ms(void);
long long	elapsed_ms(t_sim *sim);
void		smart_sleep(long long duration_ms, t_sim *sim);
int			sim_stopped(t_sim *sim);
void		sim_stop(t_sim *sim);
void		log_state(t_sim *sim, int coder_id, const char *msg);
void		set_compile_start(t_coder *me);
void		*coder_routine(void *arg);
int			init_dongles(t_sim *sim, t_config *cfg);
void		destroy_dongles(t_sim *sim);
void		assign_dongles(t_sim *sim);
int			my_turn(t_coder *me, t_dongle *d);
long long	compute_key(t_coder *me, t_dongle *d);
void		take_dongle(t_coder *me, t_dongle *d);
void		take_dongles(t_coder *me);
void		release_dongle(t_dongle *d, long long cooldown);
void		release_dongles(t_coder *me);
int			init_sim(t_sim *sim, t_config *cfg);
void		destroy_sim(t_sim *sim);
void		cleanup(t_sim *sim, int n_created);

#endif
