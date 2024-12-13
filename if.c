#include "header.h"
#include <string.h>

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
        return -1;
    }

    condition[indice] = NULL;

    int test = execute_cmd(condition + 1);
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

        if (condition[indice] == NULL || condition[indice + 1] == NULL || strcmp(condition[indice + 1], "else") != 0)
        {
            return val_retour;
        }

        debut = indice + 3;
    }

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
                condition[indice] = NULL;
                break;
            }
        }
        indice++;
    }

    val_retour = execute_cmd(condition + debut);

    return val_retour;
}

//  if true { echo a } else { echo b }