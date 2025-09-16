/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 13:06:35 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/12 12:13:42 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

char	*ft_freejoin(char *buffer, char *new)
{
	char	*tmp;

	tmp = ft_strjoin(buffer, new);
	if (!tmp)
		return (NULL);
	free(buffer);
	return (tmp);
}

char	*readfile(int fd, char *res)
{
	char	*buf;
	ssize_t	bytes;

	if (!res || !res[0])
	{
		free(res);
		res = ft_calloc(1, 1);
		if (!res)
			return (NULL);
	}
	buf = ft_calloc((BUFFER_SIZE + 1), sizeof(char));
	if (!buf)
		return (free(res), NULL);
	bytes = 1;
	while (bytes > 0)
	{
		bytes = read(fd, buf, BUFFER_SIZE);
		if (bytes == 0)
			return (free(buf), res);
		buf[bytes] = 0;
		res = ft_freejoin(res, buf);
		if (ft_strchr(res, '\n'))
			break ;
	}
	return (free(buf), res);
}

char	*nextline(char	*buf)
{
	int		i;
	int		j;
	int		slen;
	char	*line;

	slen = 0;
	if (!buf[slen] || !buf)
		return (free(buf), NULL);
	i = ft_strlen(buf);
	while (buf[slen] && buf[slen] != '\n')
		slen++;
	slen++;
	if (i <= slen)
		return (free(buf), NULL);
	else
		line = ft_calloc((i - slen + 1), sizeof(char));
	if (!line)
		return (free(buf), NULL);
	j = 0;
	while (buf[slen])
		line[j++] = buf[slen++];
	if (buf[slen] == '\n')
		line[j] = buf[slen];
	return (free(buf), line);
}

char	*makeline(char	*buf)
{
	char	*line;
	int		i;

	i = 0;
	if (!buf || !buf[0])
		return (NULL);
	while (buf[i] != '\n' && buf[i])
		i++;
	line = ft_calloc((i + 2), sizeof(char));
	if (!line)
		return (free(buf), NULL);
	i = 0;
	while (buf[i] && buf[i] != '\n')
	{
		line[i] = buf[i];
		i++;
	}
	line[i] = buf[i];
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*buf[1024];
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0 || fd > 1024)
		return (NULL);
	if (!ft_strchr(buf[fd], '\n'))
		buf[fd] = readfile(fd, buf[fd]);
	if (!buf[fd])
		return (free(buf[fd]), NULL);
	line = makeline(buf[fd]);
	buf[fd] = nextline(buf[fd]);
	return (line);
}
