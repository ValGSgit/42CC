/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 17:44:15 by kfan              #+#    #+#             */
/*   Updated: 2024/09/11 18:08:49 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	char	*str;
	char	a;

	a = c;
	str = (char *)s;
	if (a == '\0')
		return (&str[ft_strlen(s)]);
	while (*str)
	{
		if (*str == a)
			return (str);
		str++;
	}
	return (0);
}

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int	main(int argc, char **argv)
{
	(void)argc;
	printf("%s\n", strchr(argv[1], atoi(argv[2])));
	printf("%s", ft_strchr(argv[1], atoi(argv[2])));
	return (0);
}
*/
