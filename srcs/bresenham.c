/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bresenham.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:40 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/27 15:58:01 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

float	*calc_stepups(float increment[2], t_matrix a, t_matrix b)
{
	float	max;

	increment[0] = b.x - a.x;
	increment[1] = b.y - a.y;
	if (abs_value(increment[0]) >= abs_value(increment[1]))
		max = abs_value(increment[0]);
	else
		max = abs_value(increment[1]);
	increment[0] = increment[0] / max;
	increment[1] = increment[1] / max;
	return (increment);
}

// Increment[0] is X | Increment[1] is Y
void	bresenline(t_matrix a, t_matrix b, t_data *p)
{
	float	increment[2];
	int		color;

	if (!p)
		return ;
	init_coords(&a, &b, p);
	calc_stepups(increment, a, b);
	color = get_color();
	while ((int)(a.x - b.x) || (int)(a.y - b.y))
	{
		if (a.x > p->win_x || a.y > p->win_y || a.y < 0 || a.x < 0)
			break ;
		if (a.x < p->win_x && a.y < p->win_y)
			my_mlx_pixel_put(&p->img, a.x, a.y, color);
		a.x += increment[0];
		a.y += increment[1];
	}
}

void	push_image_to_window(t_data *data)
{
	void	*previous_image;

	previous_image = data->img.img;
	if (!previous_image)
		clear(data);
	mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->img.img, 0, 0);
	mlx_destroy_image(data->mlx_ptr, previous_image);
	data->img.img = mlx_new_image(data->mlx_ptr, data->win_x, data->win_y);
	data->img.addr = mlx_get_data_addr(data->img.img, &data->img.bpp,
			&data->img.line_length, &data->img.order);
}

void	put_matrix(t_matrix **matrix, t_data *data)
{
	int	y;
	int	x;

	y = 0;
	if (!matrix)
		clear(data);
	if (!data)
		clear(data);
	while (matrix[y])
	{
		x = 0;
		while (1)
		{
			if (matrix[y + 1])
				bresenline(matrix[y][x], matrix[y + 1][x], data);
			if (matrix[y][x].last)
				break ;
			if (!matrix[y][x].last)
				bresenline(matrix[y][x], matrix[y][x + 1], data);
			x++;
		}
		y++;
	}
	push_image_to_window(data);
	print_keys(data);
}

void	free_matrix(t_matrix **matrix)
{
	int	y;

	y = 0;
	if (!matrix)
		return ;
	while (matrix[y])
	{
		free(matrix[y]);
		y++;
	}
	free(matrix);
	matrix = NULL;
}
