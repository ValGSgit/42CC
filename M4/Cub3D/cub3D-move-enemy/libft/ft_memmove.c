/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 14:14:21 by kfan              #+#    #+#             */
/*   Updated: 2024/09/11 14:43:35 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char		*p1;
	const unsigned char	*p2;

	p1 = dest;
	p2 = src;
	if (p1 > p2)
	{
		while (n > 0)
		{
			n--;
			p1[n] = p2[n];
		}
	}
	else
	{
		ft_memcpy(dest, src, n);
	}
	return (dest);
}

/*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	(void)argc;
	char str1[11] = "1234567890";
	char str2[11] = "1234567890";
	char *out1;
	char *out2;
	int dest = atoi(argv[1]);
	int src = atoi(argv[2]);
	int n = atoi(argv[3]);

	out1 = memmove(&str1[dest], &str1[src], n);
	out2 = ft_memmove(&str2[dest], &str2[src], n);
	printf("%s\n", out1);
	printf("%s\n", out2);
	return (0);
}
*/
