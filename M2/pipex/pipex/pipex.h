/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 14:09:39 by vagarcia          #+#    #+#             */
/*   Updated: 2024/11/16 14:15:54 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <stdlib.h>
# include <unistd.h>
# include <stdarg.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10000
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
int		ft_printf(const char *format, ...);
int		ft_printptr(unsigned long long ptr);
int		ft_printunsigned(unsigned int n);
int		ft_printhex(unsigned int num, const char format);
size_t	intputnbr(int n);
size_t	uintputnbr(unsigned int n);
int		ft_printpercent(void);
int		ft_printstr(char *s);
size_t	ft_putchar(int c);

#endif