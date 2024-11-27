#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <signal.h>
#include "header.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUFF 1024
char prev_dir[BUFF];
volatile sig_atomic_t signal_recu = 0;

void ignore_signal(int sig)
{
    signal_recu = 1;
}

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
        return 1;
    }

    if (cmd[1] == NULL)
    {
        if (chdir(home_dir) != 0)
        {
            perror("cd home");
            return 1;
        }
    }
    else if (strcmp(cmd[1], "-") == 0)
    {
        if (prev_dir[0] == '\0')
        {
            write(1, "cd: OLDPWD not set\n",19);
            return 1;
        }
        if (chdir(prev_dir) != 0)
        {
            perror("cd -");
            return 1;
        }
    }
    else
    {
        if (chdir(cmd[1]) != 0)
        {
            perror("cd");
            return 1;
        }
    }

    strncpy(prev_dir, current_dir, sizeof(prev_dir));
    return 0;
}
int pwd(char **cmd)
{
    char current_dir[BUFF];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror("getcwd");
        return 1;
    }
    printf("%s\n", current_dir);
    return 0;
}

int ftype(char **cmd)
{
    struct stat buf;
    if (lstat(cmd[1], &buf) == -1)
    {
        perror("stat");
        return 1;
    }
        else if (S_ISREG(buf.st_mode))
    {
        write(1,"regular file\n",13);
    }
    else if (S_ISDIR(buf.st_mode))
    {
        write(1,"directory\n",10);
    }
    else if (S_ISFIFO(buf.st_mode))
    {
        write(1,"named pipe\n",11);
    }
    else if (S_ISLNK(buf.st_mode))
    {
        write(1,"symbolic link\n",14);
    }
    else
    {
        write(1,"other\n",6);
    }
    return 0;
}

int execute_cmd_externe(char **cmd)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }
    if (pid == 0)
    {
        if (execvp(cmd[0], cmd) == -1)
        {
            perror("execvp");
            exit(1);
        }
    }
    else
    {
        int res;
        if (waitpid(pid, &res, 0) == -1)
        {
            perror("waitpid");
            return 1;
        }
        if (WIFEXITED(res))
        {
            return WEXITSTATUS(res);
        }
        else
        {
            return 1;
        }

    }
    return 0;
}

int execute_cmd(char **cmd)
{
    if (!strcmp(cmd[0], "pwd"))
    {
        /* appelle pwd, return valeur de retour*/
        return pwd(cmd);
    }
    if (!strcmp(cmd[0], "cd"))
    {
        /* appelle cd, return valeur de retour*/
        return cd(cmd);
    }
    if (!strcmp(cmd[0], "ftype"))
    {
        /* appelle ftype, return valeur de retour*/
        return ftype(cmd);
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
void setup_signals()
{
    struct sigaction sa;
    sa.sa_handler = ignore_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

int main(void)
{
    int valeur_retour = 0;
    setup_signals();
    rl_outstream = stderr;
    while (1)
    {
        char *color_status;
        char *color_prompt;

        char pwd[BUFF];
        char display_pwd[26]; // 25 chars + null terminator

        color_status = (valeur_retour == 0) ? "\001\033[32m\002" : "\001\033[91m\002";
        color_prompt = "\001\033[34m\002"; // bleu par défaut

        if (getcwd(pwd, sizeof(pwd)) == NULL)
        {
            perror("getcwd");
            return -1;
        }

        size_t max_display_length = signal_recu ? 23 : 25;

        size_t pwd_len = strlen(pwd);
        if (pwd_len > max_display_length)
        {
            // Tronquer à gauche et ajouter "..."
            snprintf(display_pwd, sizeof(display_pwd), "...%s", pwd + (pwd_len - (max_display_length - 3)));
        }
        else
        {
            strncpy(display_pwd, pwd, sizeof(display_pwd));
        }
        char prompt[BUFF];
        if (signal_recu)
        {
            snprintf(prompt, sizeof(prompt), "%s[SIG]%s%s%s\001\033[00m\002$ ",
                     color_status,
                     "\001\033[0m\002",
                     color_prompt,
                     display_pwd);
            valeur_retour = 255; // Mettre à jour la valeur de retour
            signal_recu = 0;     // Réinitialiser pour le prochain prompt
        }
        else
        {
            snprintf(prompt, sizeof(prompt), "%s[%d]%s%s%s\001\033[00m\002$ ",
                     color_status,
                     valeur_retour,
                     "\001\033[0m\002",
                     color_prompt,
                     display_pwd);
        }
        char *ligne = readline(prompt);

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
