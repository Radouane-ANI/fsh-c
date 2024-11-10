#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include "header.h"
#include <unistd.h>

#define BUFF 1024
char prev_dir[BUFF];

char **separer_chaine(char *ligne)
{
    char *cpyligne = malloc(strlen(ligne) + 1);
    if (cpyligne == NULL)
    {
        perror("malloc 1 dans separer_chaine");
        exit(-1);
    }
    strcpy(cpyligne, ligne);

    int taille = 10;
    char **result = malloc(taille * sizeof(char *));
    if (result == NULL)
    {
        perror("malloc 2 dans separer_chaine");
        exit(-1);
    }

    int i = 0;
    char *saveptr;
    result[i] = strtok_r(cpyligne, " ", &saveptr);
    while (result[i] != NULL)
    {
        i++;
        result[i] = strtok_r(NULL, " ", &saveptr);
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
    free(cpyligne);
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

int execute_cmd_externe(char **cmd)
{
    return 0; // renvoie la valeur de la comande externe
}

int execute_cmd(char **cmd)
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
    return execute_cmd_externe(cmd); // pas une commande interne
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
        if (mots[0] == NULL)
        {
            free(mots);
            free(ligne);
            continue;
        }

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
        valeur_retour = execute_cmd(mots);
        free(mots);
        free(ligne);
    }
}
