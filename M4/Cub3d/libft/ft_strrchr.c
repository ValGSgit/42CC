/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 17:44:15 by kfan              #+#    #+#             */
/*   Updated: 2024/09/11 18:21:25 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*str;
	size_t	i;
	char	a;

	a = c;
	i = ft_strlen(s);
	str = (char *)s;
	if (a == '\0')
		return (&str[i]);
	while (i > 0)
	{
		i--;
		if (str[i] == a)
			return (&str[i]);
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
	printf("%s\n", strrchr(argv[1], atoi(argv[2])));
	printf("%s", ft_strrchr(argv[1], atoi(argv[2])));
	return (0);
}
*/
