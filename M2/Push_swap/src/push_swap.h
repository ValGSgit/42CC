/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 18:01:53 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/02 16:30:27 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H
# include <limits.h>
# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_node
{
	int				value;
	int				index;
	int				final_pos;
	struct s_node	*next;
}					t_node;

typedef struct s_stack
{
	t_node			*top;
}					t_stack;

void				sort_some(t_stack *a, t_stack *b, int size);
void				push(t_stack *stack, int value);
void				sort_three(t_stack *a);
void				assign_final_positions(t_stack *stack);
void				bit_sort(t_stack *a, t_stack *b);
int					calculate_moves(int position, int size);
void				ft_free(char **ar);
int					is_sorted(t_stack *a);
t_node				*create_node(int value);
void				assign_final_positions(t_stack *stack);
int					find_position(t_stack *a, int value);
int					find_min(t_stack *a);
int					find_max(t_stack *a);
int					check(char **av);
int					inrange(char **av);
int					argnum(char *av);
int					ft_strlen(const char *s);
long int			ft_atoi(const char *str);
char				*ft_strdup(const char *src);
char				*ft_strjoin(char const *s1, char const *s2);
int					ft_strncmp(const char *s1, const char *s2, size_t n);
char				*ft_substr(char const *s, unsigned int start, size_t len);
char				**ft_split(char const *s, char c);
char				**splitavs(int ac, char **av);
int					argcount(char **av);
void				rrr(t_stack *a, t_stack *b);
void				rr(t_stack *a, t_stack *b);
void				ss(t_stack *a, t_stack *b);
void				rrb(t_stack *b);
void				rra(t_stack *a);
void				rb(t_stack *b);
void				ra(t_stack *a);
void				pa(t_stack *a, t_stack *b);
void				pb(t_stack *a, t_stack *b);
void				sa(t_stack *a);
void				sb(t_stack *b);
int					stack_size(t_stack *stack);
void				rotate_to_top(t_stack *stack, int index, int is_stack_a);

#endif
