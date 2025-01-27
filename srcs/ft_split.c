/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:36:27 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/24 16:10:12 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	word_count(const char *s, char c)
{
	int	wc;
	int	flag;

	wc = 0;
	flag = 0;
	while (*s)
	{
		if (*s != c && flag == 0)
		{
			flag = 1;
			wc++;
		}
		else if (*s == c)
			flag = 0;
		s++;
	}
	return (wc);
}

void	ft_free(char **ar)
{
	int	i;

	i = 0;
	if (!ar)
		return ;
	while (ar[i])
	{
		free(ar[i]);
		i++;
	}
	if (ar)
		free(ar);
}

static char	*wordup(const char *s, int start, int finish)
{
	char	*word;
	int		i;

	i = 0;
	word = ft_calloc(sizeof(char), (finish - start + 1));
	if (!word)
		return (NULL);
	while (s && start < finish)
		word[i++] = s[start++];
	word[i] = '\0';
	return (word);
}

static void	*process_arr(char const *s, char **arr, char c)
{
	size_t	i;
	int		j;
	int		beg;

	i = 0;
	j = 0;
	beg = -1;
	while (j < word_count(s, c))
	{
		if (s[i] != c && beg < 0)
			beg = i;
		else if ((s[i] == c || i == ft_strlen(s)) && beg >= 0)
		{
			arr[j] = wordup(s, beg, i);
			if (!arr[j])
				return (ft_free(arr), NULL);
			j++;
			beg = -1;
		}
		i++;
	}
	arr[j] = NULL;
	return (arr);
}

char	**ft_split(char const *s, char c)
{
	char	**arr;

	if (!s)
		return (NULL);
	arr = ft_calloc(sizeof(char *), (word_count(s, c) + 1));
	if (!arr)
		return (ft_free(arr), NULL);
	arr = process_arr(s, arr, c);
	if (!arr)
		return (NULL);
	return (arr);
}
