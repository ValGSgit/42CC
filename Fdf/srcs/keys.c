/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keys.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:46 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/27 15:24:34 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	if_key(int key, t_data *data)
{
	if (key == XK_Escape)
		clear(data);
	return (0);
}
