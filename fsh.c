#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

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

int execute_cmd_interne(char **cmd)
{
    if (!strcmp(cmd[0], "pwd"))
    {
        /* appelle pwd, return valeur de retour*/
    }
    if (!strcmp(cmd[0], "cd"))
    {
        /* appelle cd, return valeur de retour*/
    }
    if (!strcmp(cmd[0], "ftype"))
    {
        /* appelle ftype, return valeur de retour*/
    }
    return -1; // pas une commande interne
}

int execute_cmd_externe(char **cmd)
{
    return 0; // renvoie la valeur de la comande externe
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

        valeur_retour = execute_cmd_interne(mots);
        if (valeur_retour == -1)
        {
            valeur_retour = execute_cmd_interne(mots);
        }

        free(mots);
        free(ligne);
    }
}