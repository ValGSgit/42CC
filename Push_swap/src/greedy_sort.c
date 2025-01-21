/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   greedy_sort.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 15:52:37 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/02 16:33:36 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	*create_array_from_stack(t_stack *stack, int size)
{
	int		*array;
	t_node	*current;
	int		i;

	array = (int *)malloc(size * sizeof(int));
	current = stack->top;
	i = 0;
	while (i < size)
	{
		array[i] = current->value;
		current = current->next;
		i++;
	}
	return (array);
}

void	qsort_arr(int *arr, int size)
{
	int	i;
	int	temp;
	int	j;

	i = 0;
	while (i < size - 1)
	{
		j = i + 1;
		while (j < size)
		{
			if (arr[i] > arr[j])
			{
				temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
			j++;
		}
		i++;
	}
}

void	sort_array_and_assign_positions(int *array, int size, t_stack *stack)
{
	t_node	*current;
	int		i;

	qsort_arr(array, size);
	current = stack->top;
	while (current != NULL)
	{
		i = 0;
		while (i < size)
		{
			if (current->value == array[i])
			{
				current->final_pos = i;
				current->index = i;
				break ;
			}
			i++;
		}
		current = current->next;
	}
}

void	assign_final_positions(t_stack *stack)
{
	int	size;
	int	*array;

	size = stack_size(stack);
	array = create_array_from_stack(stack, size);
	sort_array_and_assign_positions(array, size, stack);
	free(array);
}

void	bit_sort(t_stack *a, t_stack *b)
{
	int	max_bits;
	int	i;
	int	j;
	int	original_size;

	original_size = stack_size(a);
	max_bits = 0;
	i = 0;
	while ((stack_size(a) - 1) >> max_bits != 0)
		max_bits++;
	while (i < max_bits)
	{
		j = 0;
		while (j < original_size)
		{
			if (((a->top->final_pos >> i) & 1) == 0)
				pb(a, b);
			else
				ra(a);
			j++;
		}
		while (stack_size(b) > 0)
			pa(a, b);
		i++;
	}
}
