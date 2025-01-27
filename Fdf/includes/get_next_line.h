/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:23:17 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/26 11:23:17 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdlib.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif // BUFFER_SIZE

char	*get_next_line(int fd);
char	*readfile(int fd, char *res);
char	*nextline(char	*buf);
char	*makeline(char	*buf);
void	*bytecheck(ssize_t bytes, char *buf, char *line);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_freetab(char *buffer, char *new);
char	*ft_strchr(const char *str, int car);
size_t	ft_strlen(const char *str);
void	*ft_calloc(size_t nmemb, size_t size);

#endif // GET_NEXT_LINE_BONUS_H
