/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 16:13:20 by vagarcia          #+#    #+#             */
/*   Updated: 2024/11/29 13:10:04 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char **split_command(const char *command)
{
    char    **args;
    char    *cmd_copy;
    char    *token;
    size_t  count;

    count = 0;
    args = NULL;
    cmd_copy = ft_strdup(command);
    if (!cmd_copy)
    {
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    token = ft_strtok(cmd_copy, " ");
    while (token)
    {
        args = realloc(args, sizeof(char *) * (count + 1));
        if (!args)
        {
            perror("realloc");
            free(cmd_copy);
            exit(EXIT_FAILURE);
        }
        args[count++] = ft_strdup(token);
        token = ft_strtok(NULL, " ");
    }
    args = realloc(args, sizeof(char *) * (count + 1));
    args[count] = NULL;
    free(cmd_copy);
    return (args);
}

void    free_args(char **args)
{
    size_t  i;

    i = 0;
    while (args[i])
    {
        free(args[i]);
        i++;        
    }
    free(args);
}

void    ft_putstr_fd(int fd, char *s)
{
    int i;

    i = 0;
    while (s[i])
    {
        write(fd, &s[i], 1);
        i++;
    }
}

char *get_command_path(char *command, char **envp)
{   
    char    *path;
    char    *path_copy;
    char    *token;
    char    *result;
    int     i;

    i = 0;
    path = NULL;
    result = NULL;
    if (ft_strchr(command, '/'))
    { 
        if (access(command, X_OK) == 0)
            return ft_strdup(command);
        return (NULL);
    }
    while (envp[i] != NULL)
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
        {
            path = envp[i] + 5;
            break ;
        }
        i++;
    }
    if (!path) 
    {
        ft_putstr_fd(2, "PATH not found in environment\n");
        exit(EXIT_FAILURE);
    }
    path_copy = ft_strdup(path);
    token = ft_strtok(path_copy, ":");
    char cmd_path[512];
    while (token)
    {
        snprintf(cmd_path, sizeof(cmd_path), "%s/%s", token, command);
        if (access(cmd_path, X_OK) == 0)
        {
            result = ft_strdup(cmd_path);
            break ;
        }
        token = ft_strtok(NULL, ":");
    }
    free(path_copy);
    return (result);
}

void handle_timeout(int sig)
{
    (void)sig;
    write(2, "Execution timed out\n", 20);
    exit(EXIT_FAILURE);
}

int open_file(const char *filepath, int flags)
{
    int     dirfd;
    int     fd;

    if (access(filepath, F_OK) == -1) {
        perror("Error accessing file");
        exit(EXIT_FAILURE);
    }
    dirfd = open(filepath, O_RDONLY);
    if (dirfd == -1 && errno == EISDIR)
    {
        ft_putstr_fd(2, "Error: path is a directory\n");
        close(dirfd);
        exit(EXIT_FAILURE);
    }
    fd = open(filepath, flags);
    if (fd < 0) {
        if (errno == 13) {
            fprintf(stderr, "Permission denied: %s\n", filepath);
        } else {
            perror("Error opening file");
        }
        exit(EXIT_FAILURE);
    }
    close(dirfd);
    return fd;
}

// Main program
int main(int argc, char *argv[], char *envp[])
{
    int pipefd[2];
    pid_t pid1, pid2;
    int infile, outfile;

    // Validate argument count
    if (argc != 5) {
        write(2, "Usage: ./pipex <input file> <cmd1> <cmd2> <output file>\n", 55);
        exit(EXIT_FAILURE);
    }

    infile = open_file(argv[1], O_RDONLY);
    outfile = open_file(argv[4], O_WRONLY | O_CREAT | O_TRUNC);

    // Parse commands
    char **cmd1_args = split_command(argv[2]);
    char **cmd2_args = split_command(argv[3]);

    // Resolve command paths
    char *cmd1_path = get_command_path(cmd1_args[0], envp);
    char *cmd2_path = get_command_path(cmd2_args[0], envp);

    if (!cmd1_path || !cmd2_path)
    {
        ft_putstr_fd(2, "command not found: ");
        if (!cmd1_path)
            ft_putstr_fd(2, *cmd1_args);
        else if (!cmd2_path)
            ft_putstr_fd(2, *cmd2_args);
        ft_free(cmd1_args, cmd2_args, cmd1_path, cmd2_path);
        close(infile);
        close(outfile);
        exit(1);
    }
    if (pipe(pipefd) == -1)
    {
        perror("Pipe error");
        exit(EXIT_FAILURE);
    }
    if ((pid1 = fork()) == -1)
    {
        perror("Fork error for cmd1");
        close(infile);
        close(outfile);
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0) 
    {
        dup2(infile, STDIN_FILENO);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        execve(cmd1_path, cmd1_args, envp);
        perror("Execve error for cmd1");
        exit(EXIT_FAILURE);
    }
    if ((pid2 = fork()) == -1) {
        perror("Fork error for cmd2");
        close(infile);
        close(outfile);
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0)
    {
        dup2(pipefd[0], STDIN_FILENO);
        dup2(outfile, STDOUT_FILENO);
        close(pipefd[1]);
        execve(cmd2_path, cmd2_args, envp);
        perror("Execve error for cmd2");
        exit(EXIT_FAILURE);
    }
    close(infile);
    close(outfile);
    close(pipefd[0]);
    close(pipefd[1]);
    int status;
    waitpid(pid1, NULL, 0);
    waitpid(pid2, &status, 0);
    free_args(cmd1_args);
    free_args(cmd2_args);
    free(cmd1_path);
    free(cmd2_path);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return EXIT_FAILURE;
}

