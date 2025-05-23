/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:01:12 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/23 15:38:35 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	maxb;
	int		memlen;
	int		i;

	if (!s || !*s || start >= (unsigned int)ft_strlen(s))
		return (ft_strdup(""));
	maxb = (ft_strlen(s) - start);
	if (len < maxb)
		memlen = len;
	else
		memlen = maxb;
	sub = malloc(sizeof(char) * (memlen + 1));
	if (!sub)
		return (NULL);
	i = 0;
	while (i < memlen && s[i] != '\0')
		sub[i++] = s[start++];
	sub[i] = '\0';
	return (sub);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;
	unsigned char	*cs1;
	unsigned char	*cs2;

	cs1 = (unsigned char *)s1;
	cs2 = (unsigned char *)s2;
	if (n == 0)
		return (0);
	i = 0;
	while (cs1[i] == cs2[i] && cs2[i] != '\0' && i + 1 < n)
		i++;
	if (cs1[i] == cs2[i])
		return (0);
	else
		return (cs1[i] - cs2[i]);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*newstr;
	int		len1;
	int		len2;

	if (!s1 || !s2)
		return (NULL);
	newstr = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!newstr)
		return (NULL);
	len1 = 0;
	len2 = 0;
	while (len1 < ft_strlen(s1))
	{
		newstr[len1] = s1[len1];
		len1++;
	}
	while (len2 < ft_strlen(s2))
	{
		newstr[len1] = s2[len2];
		len1++;
		len2++;
	}
	newstr[len1] = '\0';
	return (newstr);
}

char	*ft_strdup(const char *src)
{
	int		i;
	char	*dup;

	i = 0;
	if (!src)
		return (NULL);
	while (src[i] != '\0')
		i++;
	dup = (char *)malloc(sizeof(char) * i + 1);
	if (!dup)
		return (free(dup), NULL);
	i = 0;
	while (src[i])
	{
		dup[i] = src[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

void	push(t_stack *stack, int value)
{
	t_node	*new_node;

	new_node = create_node(value);
	new_node->next = stack->top;
	stack->top = new_node;
}
