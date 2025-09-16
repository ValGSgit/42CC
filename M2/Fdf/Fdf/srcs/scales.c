/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scales.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:35 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/27 16:02:31 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

double	get_heights(t_data *data)
{
	static const double	height_lookup[] = {1.3, 1.0, 0.5, 0.4, 0.3, 0.2, 0.1};
	unsigned long int	index;

	index = data->dimensions / 100;
	if (index >= sizeof(height_lookup) / sizeof(height_lookup[0]))
		index = sizeof(height_lookup) / sizeof(height_lookup[0]) - 1;
	return (height_lookup[index]);
}

double	get_scale(double dimensions)
{
	static const double	scale_lookup[] = {3.0, 1.5, 1.0, 0.5, 0.4, 0.3, 0.2};
	unsigned long int	index;

	index = (int)dimensions / 100;
	if (index >= sizeof(scale_lookup) / sizeof(scale_lookup[0]))
		index = sizeof(scale_lookup) / sizeof(scale_lookup[0]) - 1;
	return (scale_lookup[index]);
}

void	print_keys(t_data *param)
{
	char	*option;
	int		color;

	color = get_color();
	option = "PRESS [ENTER] TO RESET";
	mlx_string_put(param->mlx_ptr, param->win_ptr, 869, 10, color, option);
	option = "PRESS [2/3] FOR 2D/3D MODE(BONUS)";
	mlx_string_put(param->mlx_ptr, param->win_ptr, 869, 45, color, option);
	option = "PRESS [z/x] TO ZOOM IN OR ZOOM OUT(BONUS)";
	mlx_string_put(param->mlx_ptr, param->win_ptr, 869, 80, color, option);
	option = "PRESS [c] TO CHANGE COLORS (BONUS)";
	mlx_string_put(param->mlx_ptr, param->win_ptr, 869, 115, color, option);
	option = "PRESS [q/w] KEYS TO CHANGE DEPTH (Z-SCALE)(BONUS)";
	mlx_string_put(param->mlx_ptr, param->win_ptr, 869, 150, color, option);
	option = "PRESS [a/d] KEYS FOR ROTATING THE Y AXIS(BONUS)";
	mlx_string_put(param->mlx_ptr, param->win_ptr, 869, 185, color, option);
	option = "MOVE THE PROJECT BY PRESSING THE ARROW KEYS(BONUS)";
	mlx_string_put(param->mlx_ptr, param->win_ptr, 869, 220, color, option);
	option = "PRESS [ESC] TO CLOSE THE WINDOW";
	mlx_string_put(param->mlx_ptr, param->win_ptr, 869, 255, color, option);
}
