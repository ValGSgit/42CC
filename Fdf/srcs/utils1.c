/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:34 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/21 17:26:27 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

static int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || \
	c == '\v' || c == '\f' || c == '\r');
}

int	is_valid(char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	if (s[i] == '-')
		i++;
	while (s[i] && s[i] != ',')
	{
		if (!(s[i] >= '0' && s[i] <= '9')
			&& !ft_isspace(s[i]))
			return (free(s), 0);
		i++;
	}
	if (!s[i] || s[i] == ' ')
		return (1);
	i += 1;
	if (ft_strlen(s + i) < 3)
		return (free(s), 0);
	while (s[i++])
		if (!ft_srch("0123456789abcdef", if_cap(s[i])))
			return (free(s), 0);
	return (1);
}

int	color3(t_data *params, t_pixl a, t_pixl b)
{
	if ((a.z || b.z) && a.z != b.z)
	{
		if (params->random_color)
			return (params->random_color);
		params->random_color = 0xFF0000;
		return (params->random_color);
	}
	return (YELLOW_HEX);
}

int	color4(t_data *params, t_pixl a, t_pixl b)
{
	if ((a.z || b.z) && a.z != b.z)
	{
		if (params->random_color)
			return (params->random_color);
		params->random_color = BLUE_HEX;
		return (params->random_color);
	}
	if (params->random_color2)
		return (params->random_color2);
	params->random_color2 = YELLOW_HEX;
	return (params->random_color2);
}

int	color5(t_pixl a, t_pixl b)
{
	if (a.color >= 0)
		return (a.color);
	else if (b.color >= 0)
		return (b.color);
	return (0xFF0000);
}
