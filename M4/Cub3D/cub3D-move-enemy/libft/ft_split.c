/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 10:54:40 by kfan              #+#    #+#             */
/*   Updated: 2025/03/20 18:58:24 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

//new: removed "if (s[i] != c) && s[i] != '\0')" && count = 0
//in order to return an empty str
static size_t	ft_count(char const *s, char c)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 1;
	while (s[i])
	{
		if (s[i] == c)
		{
			while (s[i] == c)
				i++;
			if (s[i] == '\0')
				return (count);
			else
				count++;
		}
		else
			i++;
	}
	return (count);
}

static char	*ft_newstring(char const *s, char c)
{
	size_t	i;
	char	*str;

	i = 0;
	while (s[i] && s[i] != c)
		i++;
	str = malloc(i + 1);
	if (!str)
		return (NULL);
	str[i] = '\0';
	while (i > 0)
	{
		i--;
		str[i] = s[i];
	}
	return (str);
}

static void	ft_free(char **array, int i)
{
	while (i >= 0)
	{
		free(array[i]);
		array[i] = NULL;
		i--;
	}
	free(array);
	array = NULL;
}

static char	**ft_array(char const *s, char c, char **array)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (i < ft_count(s, c))
	{
		while (s[j] == c)
			j++;
		array[i] = ft_newstring(s + j, c);
		if (!array[i])
		{
			ft_free(array, i - 1);
			return (NULL);
		}
		j = j + ft_strlen(array[i]) + 1;
		i++;
	}
	return (array);
}

char	**ft_split(char const *s, char c)
{
	char	**array;

	if (!s)
		return (NULL);
	array = malloc(sizeof(char *) * (ft_count(s, c) + 1));
	if (!array)
		return (NULL);
	array[ft_count(s, c)] = NULL;
	array = ft_array(s, c, array);
	return (array);
}

/*
#include <stdio.h>

int	main(int argc, char **argv)
{
	char	**ptr;
	size_t		i;

	(void)argc;
	ptr = ft_split(argv[1], argv[2][0]);
	i = 0;
	//printf("%zu\n", ft_count(argv[1], argv[2][0]));
	while (i < ft_count(argv[1], argv[2][0]))
	{
		printf("%s\n", ptr[i]);
		i++;
	}
	return (0);
}
*/
