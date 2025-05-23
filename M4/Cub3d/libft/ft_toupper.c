/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 17:28:13 by kfan              #+#    #+#             */
/*   Updated: 2024/09/20 18:40:35 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_toupper(int c)
{
	if (c >= 97 && c <= 122)
		return (c - 32);
	else
		return (c);
}

/*
#include <ctype.h>
#include <stdio.h>

int	main(int argc, char **argv)
{
	(void)argc;
	printf("%d\n", ft_toupper(argv[1][0]));
	printf("%d", ft_toupper(argv[1][0]));
	return (0);
}
*/
