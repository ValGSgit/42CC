/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/08 18:04:24 by kfan              #+#    #+#             */
/*   Updated: 2024/09/08 19:05:38 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	size_t	i;
	char	*ptr;

	i = 0;
	ptr = s;
	while (i < n)
	{
		ptr[i] = c;
		i++;
	}
	return (s);
}

/*
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int	main(int argc, char **argv)
{
	char *ptr;
	
	(void)argc;
	ptr = &argv[1][atoi(argv[2])];
	ptr = ft_memset(ptr, atoi(argv[3]), atoi(argv[4]));
	printf("%s\n", argv[1]);
	printf("%c", ptr[0]);
	return (0);
}
*/
