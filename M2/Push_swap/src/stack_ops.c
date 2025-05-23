/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack_ops.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 12:56:01 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/21 13:12:43 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	sa(t_stack *a)
{
	t_node	*first;
	t_node	*second;

	if (!a)
		return ;
	if (a->top && a->top->next)
	{
		first = a->top;
		second = a->top->next;
		first->next = second->next;
		second->next = first;
		a->top = second;
		write(1, "sa\n", 3);
	}
}

void	pb(t_stack *a, t_stack *b)
{
	t_node	*temp;

	if (!a)
		return ;
	if (a->top)
	{
		temp = a->top;
		a->top = a->top->next;
		temp->next = b->top;
		b->top = temp;
		write(1, "pb\n", 3);
	}
}

void	pa(t_stack *a, t_stack *b)
{
	t_node	*temp;

	if (!b)
		return ;
	if (b->top)
	{
		temp = b->top;
		b->top = b->top->next;
		temp->next = a->top;
		a->top = temp;
		write(1, "pa\n", 3);
	}
}

void	ra(t_stack *a)
{
	t_node	*first;
	t_node	*last;

	if (!a)
		return ;
	if (a->top && a->top->next)
	{
		first = a->top;
		last = a->top;
		while (last->next)
			last = last->next;
		a->top = first->next;
		first->next = NULL;
		last->next = first;
		write(1, "ra\n", 3);
	}
}

void	rb(t_stack *b)
{
	t_node	*first;
	t_node	*last;

	if (!b)
		return ;
	if (b->top && b->top->next)
	{
		first = b->top;
		last = b->top;
		while (last->next)
			last = last->next;
		b->top = first->next;
		first->next = NULL;
		last->next = first;
		write(1, "rb\n", 3);
	}
}
