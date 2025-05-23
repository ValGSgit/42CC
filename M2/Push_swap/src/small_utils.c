/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   small_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 14:34:10 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/21 13:12:39 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	rra(t_stack *a)
{
	t_node	*prev;
	t_node	*current;

	if (a->top && a->top->next)
	{
		prev = NULL;
		current = a->top;
		while (current->next)
		{
			prev = current;
			current = current->next;
		}
		current->next = a->top;
		a->top = current;
		prev->next = NULL;
		write(1, "rra\n", 4);
	}
}

int	calculate_moves(int position, int size)
{
	if (position <= size / 2)
		return (position);
	else
		return (size - position);
}

void	sort_three(t_stack *a)
{
	int	first;
	int	second;
	int	third;

	first = a->top->value;
	second = a->top->next->value;
	third = a->top->next->next->value;
	if (first > second && second < third && first < third)
		sa(a);
	else if (first > second && second > third)
	{
		sa(a);
		rra(a);
	}
	else if (first > second && second < third && first > third)
		ra(a);
	else if (first < second && second > third && first < third)
	{
		sa(a);
		ra(a);
	}
	else if (first < second && second > third && first > third)
		rra(a);
	return ;
}

t_node	*create_node(int value)
{
	t_node	*new_node;

	new_node = (t_node *)malloc(sizeof(t_node));
	if (!new_node)
	{
		write(2, "Failed to allocate memory", 25);
		exit(EXIT_FAILURE);
	}
	new_node->value = value;
	new_node->final_pos = 0;
	new_node->next = NULL;
	return (new_node);
}

void	swap(int *a, int *b)
{
	int	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}
