/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 13:25:56 by kfan              #+#    #+#             */
/*   Updated: 2025/05/04 11:56:59 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	size_t	i;
	char	*ptr;

	ptr = s;
	i = 0;
	while (i < n)
	{
		ptr[i] = 0;
		i++;
	}
}

/*
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	char *pin;
	int	j = 0;

	(void)argc;
	pin = &argv[1][atoi(argv[2])];
	ft_bzero(pin, atoi(argv[3]));
	printf("%s\n", argv[1]);
	while (j <= atoi(argv[3]))
	{
	printf("%s\n", (pin + j));
	j++;
	}
	return (0);
}
*/
