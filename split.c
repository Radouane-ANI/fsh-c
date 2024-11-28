#include "header.h"

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
    char *mot = strtok_r(cpyligne, " ", &saveptr);
    while (mot != NULL)
    {
        result[i] = malloc(strlen(mot) + 1);
        if (result[i] == NULL)
        {
            perror("malloc pour mot dans separer_chaine");
            exit(-1);
        }
        strcpy(result[i], mot);
        i++;

        if (i == taille)
        {
            taille *= 2;
            result = realloc(result, taille * sizeof(char *));
            if (result == NULL)
            {
                perror("realloc dans separer_chaine");
                exit(-1);
            }
        }
        mot = strtok_r(NULL, " ", &saveptr);
    }
    result[i] = NULL;
    free(cpyligne);
    return result;
}