#include "header.h"

int execute_cmd_externe(char **cmd)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }
    if (pid == 0)
    {
        if (redirection(cmd) == 1) {
            exit(1);
        }
        if (execvp(cmd[0], cmd) == -1)
        {
            perror("execvp");
            free_cmd(cmd);
            exit(1);
        }
    }
    else
    {
        int res;
        if (waitpid(pid, &res, 0) == -1)
        {
            perror("waitpid");
            return 1;
        }
        if (WIFEXITED(res))
        {
            return WEXITSTATUS(res);
        }
        else
        {
            return 1;
        }

    }
    return 0;
}