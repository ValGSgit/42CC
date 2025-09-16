/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 10:56:05 by kfan              #+#    #+#             */
/*   Updated: 2024/09/20 21:20:17 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char			*out;
	unsigned int	i;

	if (!s || !f)
		return (NULL);
	out = malloc(ft_strlen(s) + 1);
	if (!out)
		return (NULL);
	i = 0;
	while (s[i])
	{
		out[i] = f(i, s[i]);
		i++;
	}
	out[i] = '\0';
	return (out);
}

/*
char	ft_test(unsigned int i, char c)
{
	(void)i;
	if (c >= 97 && c <= 122)
		return (c - 32);
	else
		return (c);
}

#include <stdio.h>

int	main(int argc, char **argv)
{
	(void)argc;
	printf("%s", ft_strmapi(argv[1], ft_test));
	return (0);
}
*/
