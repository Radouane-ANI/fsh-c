#include "header.h"

void free_cmd(char **cmd)
{
    for (int i = 0; cmd[i] != NULL; i++)
    {
        free(cmd[i]);
    }

    free(cmd);
}