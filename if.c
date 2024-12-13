#include "header.h"
#include <string.h>

char **copie(char **source, size_t nombre_chaines)
{
    char **destination = (char **)malloc((nombre_chaines + 1) * sizeof(char *));
    if (destination == NULL)
    {
        perror("Erreur d'allocation mémoire pour le tableau");
        return NULL;
    }

    for (size_t i = 0; i < nombre_chaines; i++)
    {
        size_t longueur_chaine = strlen(source[i]) + 1;
        destination[i] = (char *)malloc(longueur_chaine * sizeof(char));

        if (destination[i] == NULL)
        {
            perror("Erreur d'allocation mémoire pour la chaîne");
            for (size_t j = 0; j < i; j++)
            {
                free(destination[j]);
            }
            free(destination);
            return NULL;
        }

        strcpy(destination[i], source[i]);
    }

    destination[nombre_chaines] = NULL;

    return destination;
}

int checkif(char **condition)
{
    if (condition == NULL || condition[0] == NULL)
    {
        return -1;
    }

    if (strcmp(condition[0], "if") != 0)
    {
        return -1;
    }

    int indice = 1;
    while (condition[indice] != NULL && strcmp(condition[indice], "{") != 0)
    {
        indice++;
    }

    if (condition[indice] == NULL)
    {
        return 2;
    }

    char **cmd_test = copie(condition + 1, indice - 1);
    if (cmd_test == NULL)
    {
        return 1;
    }
    int test = execute_cmd(cmd_test);
    free_cmd(cmd_test);
    int val_retour = 0;
    int debut;

    if (test == 0)
    {
        debut = indice + 1;
    }
    else
    {
        int nb_accolade = 1;
        while (condition[++indice] != NULL)
        {
            if (strcmp(condition[indice], "{") == 0)
            {
                nb_accolade++;
            }
            else if (strcmp(condition[indice], "}") == 0)
            {
                nb_accolade--;
                if (nb_accolade == 0)
                {
                    break;
                }
            }
        }
        if (condition[indice] == NULL || condition[indice + 1] == NULL)
        {
            return val_retour;
        }

        debut = indice + 3;
        if (strcmp(condition[indice + 1], "else") != 0)
        {
            return 2;
        }
        if (strcmp(condition[indice + 2], "{") != 0)
        {
            return 2;
        }
    }

    int fin;
    int nb_accolade = 1;
    indice = debut;
    while (condition[indice] != NULL)
    {
        if (strcmp(condition[indice], "{") == 0)
        {
            nb_accolade++;
        }
        else if (strcmp(condition[indice], "}") == 0)
        {
            nb_accolade--;
            if (nb_accolade == 0)
            {
                fin = indice;
                break;
            }
        }
        indice++;
    }
    if(condition[indice+1]!=NULL){return 2;}
    char **copie_cmd = copie(condition + debut, fin - debut);
    if (copie_cmd == NULL)
    {
        return 1;
    }

    val_retour = execute_cmd(copie_cmd);
    free_cmd(copie_cmd);

    return val_retour;
}

//  if true { echo a } else { echo b }