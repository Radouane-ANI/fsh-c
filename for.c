#include "header.h"

void static remplace(char *str, const char *mot_a_remplacer, const char *mot_de_remplacement)
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
        return 1;
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

            free_cmd(mots);
        }
    }
    closedir(dirp);

    free(rep);
    return valeur_retour;
}