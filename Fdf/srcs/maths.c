/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   maths2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:43 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/21 17:38:01 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

float	abs_value(float x)
{
	if (x < 0)
		return (-x);
	return (x);
}

int	limit_x(float x, t_data *data)
{
	if ((int)round(x) >= data->win_x)
		return ((int) x);
	return ((int) round(x));
}

int	limit_y(float y, t_data *data)
{
	if ((int) round(y) >= data->win_y)
		return ((int) y);
	return ((int) round(y));
}

void	init_coords(t_pixl *a, t_pixl *b, t_data *param)
{
	a->x *= param->scale;
	a->y *= param->scale;
	b->x *= param->scale;
	b->y *= param->scale;
	a->z *= param->z_scale;
	b->z *= param->z_scale;
	if (param->is_isometric)
	{
		a->x = (a->x - a->y) * cos(param->angle);
		a->y = (a->x + a->y) * sin(param->angle) - a->z;
		b->x = (b->x - b->y) * cos(param->angle);
		b->y = (b->x + b->y) * sin(param->angle) - b->z;
	}
	a->x += param->shift_x;
	a->y += param->shift_y;
	b->x += param->shift_x;
	b->y += param->shift_y;
}
