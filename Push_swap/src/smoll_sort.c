/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   smoll_sort.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 15:45:09 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/21 13:12:41 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	do_rot(int min_index, int size, t_stack *a)
{
	if (min_index <= size / 2)
	{
		while (min_index > 0)
		{
			ra(a);
			min_index--;
		}
	}
	else
	{
		min_index = size - min_index;
		while (min_index > 0)
		{
			rra(a);
			min_index--;
		}
	}
}

void	move_min_to_b(t_stack *a, t_stack *b)
{
	int		min;
	int		min_index;
	int		i;
	t_node	*current;

	min = a->top->value;
	current = a->top;
	min_index = 0;
	i = 0;
	while (current)
	{
		if (current->value < min)
		{
			min = current->value;
			min_index = i;
		}
		current = current->next;
		i++;
	}
	do_rot(min_index, stack_size(a), a);
	pb(a, b);
}

void	sort_five(t_stack *a, t_stack *b)
{
	move_min_to_b(a, b);
	move_min_to_b(a, b);
	sort_three(a);
	pa(a, b);
	pa(a, b);
}

void	sort_four(t_stack *a, t_stack *b)
{
	move_min_to_b(a, b);
	sort_three(a);
	pa(a, b);
}

void	sort_some(t_stack *a, t_stack *b, int size)
{
	if (size > 5)
	{
		while (size > 5)
		{
			move_min_to_b(a, b);
			size--;
		}
		sort_five(a, b);
		while (stack_size(b) > 0)
			pa(a, b);
	}
	else if (size == 4)
		sort_four(a, b);
	else if (size == 5)
		sort_five(a, b);
}
