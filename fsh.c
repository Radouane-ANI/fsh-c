#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include "header.h"
#include <unistd.h>
#include <dirent.h>

#define BUFF 1024
char prev_dir[BUFF];

void free_cmd(char **cmd)
{
    for (int i = 0; cmd[i] != NULL; i++)
    {
        free(cmd[i]);
    }

    free(cmd);
}

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

void remplace(char *str, const char *mot_a_remplacer, const char *mot_de_remplacement)
{
    char buffer[1024];
    char *ptr = str;
    char *position;
    int longueur_mot = strlen(mot_a_remplacer);

    buffer[0] = '\0';

    while ((position = strstr(ptr, mot_a_remplacer)) != NULL)
    {
        strncat(buffer, ptr, position - ptr);
        strcat(buffer, mot_de_remplacement);
        ptr = position + longueur_mot;
    }

    strcat(buffer, ptr);
    strcpy(str, buffer);
}

int checkfor(char *ligne)
{
    int valeur_retour = 0;
    char c;
    char cmd[BUFF];
    char *rep = malloc(BUFF * sizeof(char));
    if (rep == NULL)
    {
        perror("malloc");
        exit(1);
    }
    int match = sscanf(ligne, "for %c in %[^ ] { %[^}]", &c, rep, cmd);
    if (match != 3)
    {
        free(rep);
        return -1;
    }
    struct dirent *entry;
    DIR *dirp = opendir(rep);
    if (dirp == NULL)
    {
        free(rep);
        perror("opendir");
        exit(1);
    }
    char ref[BUFF];
    char *remplacer = strchr(cmd, c);
    while ((entry = readdir(dirp)))
    {
        if (entry->d_name[0] != '.')
        {
            char cmd_temp[BUFF];
            strcpy(cmd_temp, cmd);

            if (remplacer != NULL)
            {
                char dollarc[3] = "$";
                dollarc[1] = c;
                dollarc[2] = '\0';
                snprintf(ref, BUFF, "%s/%s", rep, entry->d_name);
                remplace(cmd_temp, dollarc, ref);
            }
            char **mots = separer_chaine(cmd_temp);
            if (mots[0] == NULL)
            {
                free_cmd(mots);
                continue;
            }

            int val = execute_cmd(mots);
            if (val > valeur_retour)
            {
                valeur_retour = val;
            }

            free(mots);
        }
    }
    closedir(dirp);

    free(rep);
    return valeur_retour;
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

        int val = checkfor(ligne);
        if (val != -1)
        {
            valeur_retour = val;
            free(ligne);
            continue;
        }

        char **mots = separer_chaine(ligne);
        if (mots[0] == NULL)
        {
            free_cmd(mots);
            free(ligne);
            continue;
        }

        if (!strcmp(mots[0], "exit"))
        {
            if (mots[1] != NULL)
            {
                valeur_retour = atoi(mots[1]);
            }
            free_cmd(mots);
            free(ligne);
            return valeur_retour;
        }
        /*
        char curr[BUFF];
        getcwd(curr, sizeof(curr));
        printf("%s\n", curr);
        */
        valeur_retour = execute_cmd(mots);
        free_cmd(mots);
        free(ligne);
    }
}
