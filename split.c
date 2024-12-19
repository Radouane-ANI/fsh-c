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

    int taille = 100;
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

int trouve_accolade_fermante(char **cmd, int debut)
{
    int nb_accolade = 1;
    int i = debut + 1;
    while (cmd[i] != NULL)
    {
        if (strcmp(cmd[i], "{") == 0)
        {
            nb_accolade++;
        } else if (strcmp(cmd[i], "}") == 0)
        {
            nb_accolade--;
            if (nb_accolade == 0)
            {
                return i;
            }
        }
        i++;
    }
    return -1;
}

char ***separer_commandes(char **cmd)
{
    int nb_cmd = 1;
    int i = 0;
    
    while (cmd[i] != NULL)
    {
        if (strcmp(cmd[i], "for") == 0 || strcmp(cmd[i], "if") == 0)
        {
            // Chercher l'accolade ouvrante
            while (cmd[i] != NULL && strcmp(cmd[i], "{") != 0)
            {
                i++;
            }
            if (cmd[i] != NULL) 
            {
                int fin = trouve_accolade_fermante(cmd, i);
                if (fin != -1)
                {
                    i = fin;
                }
            }
        }
        else if (strcmp(cmd[i], ";") == 0)
        {
            nb_cmd++;
        }
        i++;
    }
    
    char ***cmds = malloc((nb_cmd + 1) * sizeof(char **));
    int cmd_courante = 0;
    int debut = 0;
    i = 0;

    while (cmd[i] != NULL)
    {
        if (strcmp(cmd[i], "for") == 0 || strcmp(cmd[i], "if") == 0)
        {
            while (cmd[i] != NULL && strcmp(cmd[i], "{") != 0)
            {
                i++;
            }
            if (cmd[i] != NULL)
            {
                int fin = trouve_accolade_fermante(cmd, i);
                if (fin != -1)
                {
                    i = fin;
                }
            }
        }
        else if (strcmp(cmd[i], ";") == 0)
        {
            int taille = i - debut;
            cmds[cmd_courante] = malloc((taille + 1) * sizeof(char *));
            for (int j = 0; j < taille; j++)
            {
                cmds[cmd_courante][j] = strdup(cmd[debut + j]);
            }
            cmds[cmd_courante][taille] = NULL;
            cmd_courante++;
            debut = i + 1;
        }
        i++;
    }
    if (debut < i)
    {
        int taille = i - debut;
        cmds[cmd_courante] = malloc((taille + 1) * sizeof(char *));
        for (int j = 0; j < taille; j++)
        {
            cmds[cmd_courante][j] = strdup(cmd[debut + j]);
        }
        cmds[cmd_courante][taille] = NULL;
        cmd_courante++;
    }
    
    cmds[cmd_courante] = NULL;
    return cmds;
}