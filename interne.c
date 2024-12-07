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
    char current_dir[BUFF];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror("getcwd");
        return 1;
    }
    write(1, current_dir, strlen(current_dir));
    write(1, "\n", 1);
    return 0;
}

int static ftype(char **cmd)
{
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
    int val = checkfor(cmd);
    if (val != -1)
    {
        return val;
    }
    val = checkif(cmd);
    if (val != -1)
    {
        return val;
    }
    if (!strcmp(cmd[0], "pwd"))
    {
        /* appelle pwd, return valeur de retour*/
        return pwd(cmd);
    }
    if (!strcmp(cmd[0], "cd"))
    {
        /* appelle cd, return valeur de retour*/
        return cd(cmd);
    }
    if (!strcmp(cmd[0], "ftype"))
    {
        /* appelle ftype, return valeur de retour*/
        return ftype(cmd);
    }
    return execute_cmd_externe(cmd); // pas une commande interne
}