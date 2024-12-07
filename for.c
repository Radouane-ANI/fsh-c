#include "header.h"

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

char *reconstruit(char **ligne)
{
    int total_length = 0;
    int count = 0;
    int nb_accolade = 1;
    for (int i = 0; ligne[i] != NULL; i++)
    {
        if (strcmp(ligne[i], "{") == 0)
        {
            nb_accolade++;
        }
        else if (strcmp(ligne[i], "}") == 0)
        {
            nb_accolade--;
            if (nb_accolade == 0)
            {
                break;
            }
        }
        total_length += strlen(ligne[i]);
        count++;
    }
    total_length += (count - 1);

    char *result = (char *)malloc((total_length + 1) * sizeof(char));
    if (!result)
    {
        exit(EXIT_FAILURE);
    }
    nb_accolade = 1;
    result[0] = '\0';
    for (int i = 0; ligne[i] != NULL; i++)
    {
        if (strcmp(ligne[i], "{") == 0)
        {
            nb_accolade++;
        }
        else if (strcmp(ligne[i], "}") == 0)
        {
            nb_accolade--;
            if (nb_accolade == 0)
            {
                strcat(result, "\0");
                break;
            }
        }
        strcat(result, ligne[i]);
        if (ligne[i + 1] != NULL)
        {
            strcat(result, " ");
        }
    }
    return result;
}

int parcours_rep(char c, char *cmd, char *rep, int option_A, int option_r, char *option_e, char option_t, int option_p)
{
    int valeur_retour = 0;
    struct dirent *entry;
    DIR *dirp = opendir(rep);
    if (dirp == NULL)
    {
        free(cmd);
        perror("opendir");
        return 1;
    }
    char ref[BUFF];
    char *remplacer = strchr(cmd, c);
    while ((entry = readdir(dirp)))
    {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
        {
            continue;
        }
        // Ignorer les fichiers cachés si -A n'est pas activé
        if (entry->d_name[0] != '.' || option_A)
        {
            snprintf(ref, BUFF, "%s/%s", rep, entry->d_name);
            // Filtrage par extension (-e EXT)
            if (option_e != NULL)
            {
                size_t len_name = strlen(entry->d_name);
                size_t len_ext = strlen(option_e);
                if (len_name < len_ext || strcmp(entry->d_name + len_name - len_ext, option_e) != 0)
                {
                    continue;
                }
                ref[strlen(ref) - len_ext - 1] = '\0';
            }

            char cmd_temp[BUFF];
            strcpy(cmd_temp, cmd);

            if (remplacer != NULL)
            {
                char dollarc[3] = "$";
                dollarc[1] = c;
                dollarc[2] = '\0';
                remplace(cmd_temp, dollarc, ref);
            }
            struct stat st;
            if (lstat(ref, &st) != 0)
            {
                perror("stat");
                return -1;
            }
            // Filtrage par type (-t TYPE)
            if ((option_t == 'f' && !S_ISREG(st.st_mode)) ||
                (option_t == 'd' && !S_ISDIR(st.st_mode)) ||
                (option_t == 'l' && !S_ISLNK(st.st_mode)) ||
                (option_t == 'p' && !S_ISFIFO(st.st_mode)))
            {
                continue;
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
            // Parcours récursif
            if (option_r && (st.st_mode & __S_IFMT) == __S_IFDIR)
            {
                val = parcours_rep(c, cmd, ref, option_A, option_r, option_e, option_t, option_p);
                if (val > valeur_retour)
                {
                    valeur_retour = val;
                }
            }
        }
    }
    closedir(dirp);

    return valeur_retour;
}

int checkfor(char **boucle)
{
    char c;
    char *cmd = "";
    char *rep;

    int option_A = 0;      // Inclure les fichiers cachés
    int option_r = 0;      // Parcours récursif
    char *option_e = NULL; // Extension pour le filtrage
    char option_t = 0;     // Type de fichier pour le filtrage
    int option_p = -1;     // Limite max des fichiers

    if (boucle == NULL || boucle[0] == NULL)
    {
        return -1;
    }
    if (strcmp(boucle[0], "for") || strcmp(boucle[2], "in"))
    {
        return -1;
    }

    if (boucle[1] != NULL && boucle[3] != NULL)
    {
        c = boucle[1][0];
        rep = boucle[3];
    }
    else
    {
        return -1;
    }

    for (int i = 4; boucle[i] != NULL; i++)
    {
        if (!strcmp(boucle[i], "-A"))
        {
            option_A = 1;
        }
        else if (!strcmp(boucle[i], "-r"))
        {
            option_r = 1;
        }
        else if (!strcmp(boucle[i], "-e") && boucle[i + 1] != NULL)
        {
            option_e = boucle[++i];
        }
        else if (!strcmp(boucle[i], "-t") && boucle[i + 1] != NULL)
        {
            option_t = boucle[++i][0];
        }
        else if (!strcmp(boucle[i], "-p") && boucle[i + 1] != NULL)
        {
            option_p = atoi(boucle[++i]);
        }
        else if (!strcmp(boucle[i], "{") && boucle[i + 1] != NULL)
        {
            cmd = reconstruit(boucle + i + 1);
            break;
        }
    }

    if (cmd[0] == '\0')
    {
        return -1;
    }
    int val = parcours_rep(c, cmd, rep, option_A, option_r, option_e, option_t, option_p);
    free(cmd);
    return val;
}