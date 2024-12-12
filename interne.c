#include "header.h"

char prev_dir[BUFF];

int static cd(char **cmd)
{
    char *home_dir = getenv("HOME");
    char current_dir[BUFF];

    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror("getcwd");
        return 1;
    }

    if (cmd[1] == NULL)
    {
        if (chdir(home_dir) != 0)
        {
            perror("cd home");
            return 1;
        }
    }
    else if (cmd[2] != NULL)
    {
        write(2, "cd: too many arguments\n", 24);
        return 1;
    }
    else if (strcmp(cmd[1], "-") == 0)
    {
        if (prev_dir[0] == '\0')
        {
            write(1, "cd: OLDPWD not set\n",19);
            return 1;
        }
        if (chdir(prev_dir) != 0)
        {
            perror("cd -");
            return 1;
        }
    }
    else
    {
        if (chdir(cmd[1]) != 0)
        {
            perror("cd");
            return 1;
        }
    }

    strncpy(prev_dir, current_dir, sizeof(prev_dir));
    return 0;
}

int static pwd(char **cmd)
{
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stderr = dup(STDERR_FILENO);

    int redir_result = redirection(cmd);
    if (redir_result == 1) {
        if (saved_stdout != -1) {
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
        }
        if (saved_stdin != -1) {
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdin);
        }
        if (saved_stderr != -1) {
            dup2(saved_stderr, STDERR_FILENO);
            close(saved_stderr);
        }
        return 1;
    }

    // Vérifier les arguments seulement s'il n'y a pas de redirection
    if (redir_result == 2 && cmd[1] != NULL) {
        write(2, "pwd: invalid argument\n", 23);
        // Restaurer les descripteurs de fichiers d'origine
        if (saved_stdout != -1) {
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
        }
        if (saved_stdin != -1) {
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdin);
        }
        if (saved_stderr != -1) {
            dup2(saved_stderr, STDERR_FILENO);
            close(saved_stderr);
        }
        return 1;
    }

    char current_dir[BUFF];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("getcwd");
        // Restaurer les descripteurs de fichiers d'origine
        if (saved_stdout != -1) {
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
        }
        if (saved_stdin != -1) {
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdin);
        }
        if (saved_stderr != -1) {
            dup2(saved_stderr, STDERR_FILENO);
            close(saved_stderr);
        }
        return 1;
    }

    write(1, current_dir, strlen(current_dir));
    write(1, "\n", 1);

    // Restaurer les descripteurs de fichiers d'origine
    if (saved_stdout != -1) {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }
    if (saved_stdin != -1) {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }
    if (saved_stderr != -1) {
        dup2(saved_stderr, STDERR_FILENO);
        close(saved_stderr);
    }

    return 0;
}

int static ftype(char **cmd)
{
     if (cmd[2] != NULL)
    {
        write(2, "ftype: too many arguments\n", 27);
        return 1;
    }
    struct stat buf;
    if (lstat(cmd[1], &buf) == -1)
    {
        perror("stat");
        return 1;
    }
    else if (S_ISREG(buf.st_mode))
    {
        write(1,"regular file\n",13);
    }
    else if (S_ISDIR(buf.st_mode))
    {
        write(1,"directory\n",10);
    }
    else if (S_ISFIFO(buf.st_mode))
    {
        write(1,"named pipe\n",11);
    }
    else if (S_ISLNK(buf.st_mode))
    {
        write(1,"symbolic link\n",14);
    }
    else
    {
        write(1,"other\n",6);
    }
    return 0;
}

int execute_cmd(char **cmd)
{
    int saved_stdin = -1, saved_stdout = -1;

    int result = -1;
    if (!strcmp(cmd[0], "pwd"))
    {
        result = pwd(cmd);
    }
    else if (!strcmp(cmd[0], "cd"))
    {
        result = cd(cmd);
    }
    else if (!strcmp(cmd[0], "ftype"))
    {
        result = ftype(cmd);
    }
    else
    {
        result = execute_cmd_externe(cmd); // pas une commande interne
    }
  

    // Restaurer les descripteurs de fichiers si nécessaire
    if (saved_stdout != -1)
    {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }
    if (saved_stdin != -1)
    {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }

    return result;
}