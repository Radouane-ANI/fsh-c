#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <signal.h>
#include "header.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFF 1024
char prev_dir[BUFF];
volatile sig_atomic_t signal_recu = 0;

void ignore_signal(int sig) {
    signal_recu = 1;
}

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
int pwd (char **cmd){
    char current_dir[BUFF];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror("getcwd");
        return -1;
    }
    printf("%s\n", current_dir);
    return 0;
}

int  ftype(char **cmd)
{
    struct stat buf;
    if (stat(cmd[1], &buf) == -1)
    {
        perror("stat");
        return -1;
    }
    if (S_ISREG(buf.st_mode))
    {
        printf("regular file\n");
    }
    else if (S_ISDIR(buf.st_mode))
    {
        printf("directory\n");
    }
    else if (S_ISFIFO(buf.st_mode))
    {
        printf("named pipe\n");
    }
    else if (S_ISLNK(buf.st_mode))
    {
        printf("symbolic link\n");
    }
    else
    {
        printf("other\n");
    }
    return 0;   
}


int execute_cmd_interne(char **cmd)
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
    return -1; // pas une commande interne
}

int execute_cmd_externe(char **cmd)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
    }
    else if (pid == 0)
    {
        if (execvp(cmd[0], cmd) == -1)
        {
            perror("execvp");
            return -1;
        }
    }
    else
    {
        if (waitpid(pid, NULL, 0) == -1)
        {
            perror("waitpid");
            return -1;
        }
    }
    return 0;
}
void setup_signals() {
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
    while (1)
    {
        char *color_status;
        char *color_prompt;
        
        char pwd[BUFF];
        char display_pwd[26]; // 25 chars + null terminator

        color_status = (valeur_retour == 0) ? "\001\033[32m\002" : "\001\033[91m\002";
        color_prompt = "\001\033[34m\002";  // bleu par défaut

        if (getcwd(pwd, sizeof(pwd)) == NULL)
        {
            perror("getcwd");  
            return -1;
        }

        size_t max_display_length = signal_recu ? 23 : 25;
        
        size_t pwd_len = strlen(pwd);
        if (pwd_len > max_display_length) {
            // Tronquer à gauche et ajouter "..."
            snprintf(display_pwd, sizeof(display_pwd), "...%s", pwd + (pwd_len - (max_display_length - 3)));
        } else {
            strncpy(display_pwd, pwd, sizeof(display_pwd));
        }
        char prompt[BUFF];
        if (signal_recu) {
            snprintf(prompt, sizeof(prompt), "%s[SIG]%s%s%s\001\033[00m\002$ ", 
                color_status,
                "\001\033[0m\002",
                color_prompt,
                display_pwd
            );
            valeur_retour = 255;  // Mettre à jour la valeur de retour
            signal_recu = 0;      // Réinitialiser pour le prochain prompt
        } else {
            snprintf(prompt, sizeof(prompt), "%s[%d]%s%s%s\001\033[00m\002$ ", 
                color_status,
                valeur_retour,
                "\001\033[0m\002",
                color_prompt,
                display_pwd
            );
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
            valeur_retour = execute_cmd_externe(mots);
        }

        free(mots);
        free(ligne);
    }
}
