/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keys.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:46 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/21 17:34:13 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	is_key(int key)
{
	return (key == XK_Escape);
}

void	do_key(int key, t_data *data)
{
	if (key == XK_Escape)
		ft_exit(data);
}

int	if_key(int key, t_data *data)
{
	if (is_key(key))
	{
		mlx_clear_window(data->mlx_ptr, data->win_ptr);
		do_key(key, data);
		print_option(data);
		draw(data->matrix, data);
	}
	return (0);
}
