/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 10:43:53 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/11 16:25:05 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex_bonus.h"

void	ft_putendl_fd(char *s, int fd)
{
	int	i;

	i = 0;
	if (!s)
		return ;
	while (s[i])
	{
		write(fd, &s[i], 1);
		i++;
	}
	ft_putstr_fd("\n", fd);
}

void	ft_putstr_fd(char *s, int fd)
{
	int	i;

	i = 0;
	if (!*s)
		return ;
	while (s[i] != '\0')
	{
		write(fd, &s[i], 1);
		i++;
	}
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

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	maxb;
	int		memlen;
	int		i;

	if (!s || !*s || start >= ft_strlen(s))
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

char	*ft_strdup(const char *src)
{
	int		i;
	char	*dup;

	i = 0;
	while (src[i] != '\0')
		i++;
	dup = (char *)malloc(sizeof(*dup) * i + 1);
	if (dup == NULL)
		return (NULL);
	i = 0;
	while (src[i])
	{
		dup[i] = src[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}
