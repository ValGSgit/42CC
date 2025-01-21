/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 10:50:47 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/11 16:25:02 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex_bonus.h"

static int	ft_wc(const char *s, char c)
{
	int	i;
	int	trigger;

	i = 0;
	trigger = 0;
	while (*s)
	{
		if (*s != c && trigger == 0)
		{
			trigger = 1;
			i++;
		}
		else if (*s == c)
			trigger = 0;
		s++;
	}
	return (i);
}

void	free_tab(char **array)
{
	int	i;

	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

static char	*wordup(const char *s, int start, int finish)
{
	char	*word;
	int		i;

	i = 0;
	word = malloc(sizeof(char) * (finish - start + 1));
	if (!word)
		return (free(word), NULL);
	while (start < finish)
		word[i++] = s[start++];
	word[i] = '\0';
	return (word);
}

static void	*ft_woodchipper(char const *s, char **tab, char c)
{
	size_t	i;
	int		j;
	int		beg;

	i = 0;
	j = 0;
	beg = -1;
	while (j < ft_wc(s, c))
	{
		if (s[i] != c && beg < 0)
			beg = i;
		else if ((s[i] == c || i == ft_strlen(s)) && beg >= 0)
		{
			tab[j] = wordup(s, beg, i);
			if (!tab[j])
				return (free_tab(tab), free(tab), NULL);
			j++;
			beg = -1;
		}
		i++;
	}
	tab[j] = NULL;
	return (tab);
}

char	**ft_split(char const *s, char c)
{
	char	**tab;

	if (!s)
		return (NULL);
	tab = malloc((sizeof(char *) * (ft_wc(s, c) + 1)));
	if (!tab)
		return (free_tab(tab), NULL);
	tab = ft_woodchipper(s, tab, c);
	if (!tab)
		return (free_tab(tab), NULL);
	return (tab);
}
