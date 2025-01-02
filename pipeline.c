#include "header.h"

static void cleanup_pipeline(char ***cmds, int *tubes[][2], pid_t *pids, int nb_cmds, int tubes_created)
{
    for (int i = 0; i < tubes_created; i++)
    {
        close(tubes[i][0]);
        close(tubes[i][1]);
    }
    if (cmds) {
        for (int i = 0; cmds[i] != NULL; i++)
        {
            free_cmd(cmds[i]);
        }
        free(cmds);
    }
    if (pids != NULL)
    {
        if (pids)
        {
            for (int i = 0; i < nb_cmds; i++)
            {
                if (pids[i] > 0)
                {
                    kill(pids[i], SIGTERM);
                }
            }
            free(pids);
        }
    }
}

int execute_pipeline(char **cmd)
{
    int nb_cmds = 1;
    int i = 0;
    int in_structure = 0;
    int accolades = 0;
    
    while (cmd[i] != NULL)
    {
        if (strcmp(cmd[i], "{") == 0)
        {
            in_structure = 1;
            accolades++;
        }
        else if (strcmp(cmd[i], "}") == 0)
        {
            accolades--;
            if (accolades == 0) in_structure = 0;
        }
        else if (!in_structure && strcmp(cmd[i], "|") == 0)
            nb_cmds++;
        i++;
    }
    
    if (nb_cmds == 1)
        return execute_cmd_interne(cmd);
    
    char ***cmds = malloc((nb_cmds + 1) * sizeof(char **));
    int cmd_courante = 0;
    int debut = 0;
    i = 0;
    in_structure = 0;
    accolades = 0;
    
    while (cmd[i] != NULL)
    {
        if (strcmp(cmd[i], "{") == 0)
        {
            in_structure = 1;
            accolades++;
        }
        else if (strcmp(cmd[i], "}") == 0)
        {
            accolades--;
            if (accolades == 0) in_structure = 0;
        }
        else if (!in_structure && strcmp(cmd[i], "|") == 0)
        {
            int len = i - debut;
            cmds[cmd_courante] = malloc((len + 1) * sizeof(char *));
            for (int j = 0; j < len; j++)
            {
                cmds[cmd_courante][j] = strdup(cmd[debut + j]);
            }
            cmds[cmd_courante][len] = NULL;
            cmd_courante++;
            debut = i + 1;
        }
        i++;
    }
    
    if (cmd[debut] != NULL)
    {
        int len = i - debut;
        cmds[cmd_courante] = malloc((len + 1) * sizeof(char *));
        for (int j = 0; j < len; j++) {
            cmds[cmd_courante][j] = strdup(cmd[debut + j]);
        }
        cmds[cmd_courante][len] = NULL;
        cmd_courante++;
    }
    cmds[cmd_courante] = NULL;

    int tubes[nb_cmds - 1][2];
    for (i = 0; i < nb_cmds - 1; i++)
    {
        if (pipe(tubes[i]) == -1)
        {
            perror("pipe");
            cleanup_pipeline(cmds, tubes, NULL, nb_cmds, i);
            return 1;
        }
    }
    

    pid_t *pids = malloc(nb_cmds * sizeof(pid_t));
    for (i = 0; i < nb_cmds; i++)
    {
        pids[i] = fork();
        if (pids[i] == -1)
        {
            perror("fork");
            cleanup_pipeline(cmds, tubes, pids, nb_cmds, i);
            return 1;
        }
        
        if (pids[i] == 0)
        {
            if (i > 0)
            {
                if (dup2(tubes[i-1][0], STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    exit(1);
                }
            }
            if (i < nb_cmds-1)
            {
                if (dup2(tubes[i][1], STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    exit(1);
                }
            }
            for (int j = 0; j < nb_cmds - 1; j++)
            {
                close(tubes[j][0]);
                close(tubes[j][1]);
            }
            exit(execute_cmd_interne(cmds[i]));
        }
    }
    
    for (i = 0; i < nb_cmds - 1; i++)
    {
        close(tubes[i][0]);
        close(tubes[i][1]);
    }
    
    int status;
    waitpid(pids[nb_cmds - 1], &status, 0);
    int valeur_retour = WEXITSTATUS(status);
    
    for (i = 0; i < nb_cmds - 1; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
    
    for (i = 0; i < nb_cmds; i++)
    {
        free_cmd(cmds[i]);
    }
    free(cmds);
    free(pids);
    
    return valeur_retour;
}