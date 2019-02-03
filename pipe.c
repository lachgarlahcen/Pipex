/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysalihi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/03 19:58:22 by ysalihi           #+#    #+#             */
/*   Updated: 2019/02/03 19:58:37 by ysalihi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "libft/libft.h"
#include <fcntl.h>

char	*get_path(char **paths, char *cmd)
{
	int			i;
	char		*path;
	char		*part_path;

	i = 0;
	while (paths[i] && cmd[1] != '/')
	{
		part_path = ft_strjoin(paths[i], "/");
		path = ft_strjoin(part_path, cmd);
		ft_strdel(&part_path);
		if (access(path, F_OK) == 0)
			return (path);
		ft_strdel(&path);
		i++;
	}
	if (cmd[0] == '/')
	{
		if (access(cmd, F_OK) == 0)
			return (cmd);
	}
	return (NULL);
}

void	second_cmd_function(int *tub, char *argv4, char **sec_cmd, char **env)
{
	int		fd_out;
	char	**split_path;
	char	*path2;

	split_path = ft_strsplit(env[12] + 5, ':');
	path2 = get_path(split_path, sec_cmd[0]);
	close(tub[1]);
	dup2(tub[0], STDIN_FILENO);
	close(tub[0]);
	if ((fd_out = open(argv4, O_WRONLY | O_TRUNC | O_CREAT, 0644)) < 0)
	{
		ft_putstr("error opening: ");
		ft_putstr(argv4);
		ft_putstr("\n");
		exit(EXIT_FAILURE);
	}
	dup2(fd_out, STDOUT_FILENO);
	close(fd_out);
	if (execve(path2, sec_cmd, env) == -1)
	{
		ft_putstr("Erreur dans execve()2\n");
		exit(EXIT_FAILURE);
	}
}

void	first_cmd_function(int *tub, char *argv1, char **first_cmd, char **env)
{
	int		fd_in;
	char	**split_path;
	char	*path1;

	split_path = ft_strsplit(env[12] + 5, ':');
	path1 = get_path(split_path, first_cmd[0]);
	if ((fd_in = open(argv1, O_RDONLY)) < 0)
	{
		ft_putstr("no such file or directory: ");
		ft_putstr(argv1);
		ft_putstr("\n");
		exit(EXIT_FAILURE);
	}
	dup2(fd_in, STDIN_FILENO);
	close(fd_in);
	close(tub[0]);
	dup2(tub[1], STDOUT_FILENO);
	close(tub[1]);
	if (execve(path1, first_cmd, env) == -1)
	{
		ft_putstr("Erreur dans execve()1\n");
		exit(EXIT_FAILURE);
	}
}

void	print_error(char *error)
{
	ft_putstr("error: ");
	ft_putstr(error);
	exit(EXIT_FAILURE);
}

int		main(int argc, char **argv, char **envp)
{
	int		tuyau[2];
	char	**first_cmd;
	char	**secend_cmd;
	pid_t	frk;

	if (argc == 5)
	{
		first_cmd = ft_strsplit(argv[2], ' ');
		secend_cmd = ft_strsplit(argv[3], ' ');
		if (pipe(tuyau) == -1)
			print_error("Erreur dans pipe().\n");
		frk = fork();
		if (frk == -1)
			print_error("Erreur dans fork().\n");
		else if (frk == 0)
			first_cmd_function(tuyau, argv[1], first_cmd, envp);
		else
			second_cmd_function(tuyau, argv[4], secend_cmd, envp);
		exit(EXIT_SUCCESS);
	}
	print_error("usage : ./pipex infile cmd1 cmd2 outfile\n");
	return (0);
}
