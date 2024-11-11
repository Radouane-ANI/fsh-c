#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include "header.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFF 1024
char prev_dir[BUFF];

char **separer_chaine(char *ligne)
{
    int taille = 100;
    char **result = malloc(taille * sizeof(char *));
    if (result == NULL)
    {
        perror("malloc dans separer_chaine");
        exit(-1);
    }

    int i = 0;

    result[i] = strtok(ligne, " ");
    while (result[i] != NULL)
    {
        i++;
        result[i] = strtok(NULL, " ");
        if (i == taille - 1)
        {
            taille *= 2;
            result = realloc(result, taille * sizeof(char *));
            if (result == NULL)
            {
                perror("realloc dans separer_chaine");
                exit(-1);
            }
        }
    }
    return result;
}

int cd(char **cmd)
{
    char *home_dir = getenv("HOME");
    char current_dir[BUFF];

    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror("getcwd");
        return -1;
    }

    if (cmd[1] == NULL)
    {
        if (chdir(home_dir) != 0)
        {
            perror("cd home");
            return -1;
        }
    }
    else if (strcmp(cmd[1], "-") == 0)
    {
        if (prev_dir[0] == '\0')
        {
            fprintf(stderr, "cd: OLDPWD not set\n");
            return -1;
        }
        if (chdir(prev_dir) != 0)
        {
            perror("cd -");
            return -1;
        }
        printf("%s\n", prev_dir);
    }
    else
    {
        if (chdir(cmd[1]) != 0)
        {
            perror("cd");
            return -1;
        }
    }

    strncpy(prev_dir, current_dir, sizeof(prev_dir));
    return 0;
}

int execute_cmd_interne(char **cmd)
{
    if (!strcmp(cmd[0], "pwd"))
    {
        /* appelle pwd, return valeur de retour*/
    }
    if (!strcmp(cmd[0], "cd"))
    {
        /* appelle cd, return valeur de retour*/
		return cd(cmd);
    }
    if (!strcmp(cmd[0], "ftype"))
    {
        /* appelle ftype, return valeur de retour*/
    }
    return -1; // pas une commande interne
}

int execute_cmd_externe(char **cmd)
{
        pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
    }
    else if (pid == 0)
    {
        if (execvp(cmd[0], cmd) == -1)
        {
            perror("execvp");
            return -1;
        }
    }
    else
    {
        if (waitpid(pid, NULL, 0) == -1)
        {
            perror("waitpid");
            return -1;
        }
    }
    return 0;
}

int main(void)
{
    while (1)
    {
        int valeur_retour = 0;

        char *ligne = readline("mon joli prompt $ ");

        if (ligne == NULL)
        {
            return valeur_retour;
        }
        if (ligne[0] == '\0')
        {
            free(ligne);
            continue;
        }
        add_history(ligne);

        char **mots = separer_chaine(ligne);
        if (!strcmp(mots[0], "exit"))
        {
            if (mots[1] != NULL)
            {
                valeur_retour = atoi(mots[1]);
            }
            free(mots);
            free(ligne);
            return valeur_retour;
        }
	/*
	char curr[BUFF];
	getcwd(curr, sizeof(curr));
	printf("%s\n", curr);
	*/
        valeur_retour = execute_cmd_interne(mots);
        if (valeur_retour == -1)
        {
            valeur_retour = execute_cmd_externe(mots);
        }

        free(mots);
        free(ligne);
    }
}
