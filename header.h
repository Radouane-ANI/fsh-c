#ifndef HEADER_H
#define HEADER_H

#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUFF 1024
extern volatile sig_atomic_t signal_recu;
extern char prompt[BUFF];

void free_cmd(char **cmd);
void ignore_signal(int sig);
char **separer_chaine(char *ligne);
int execute_cmd(char **cmd);
int execute_cmd_externe(char **cmd);
int execute_cmd_externe(char **cmd);
int checkfor(char **boucle);
int checkif(char **condition);
char **separer_chaine(char *ligne);
void setup_signals();
int setup_prompt(int valeur_retour);
int redirection(char **cmd);
int loop();
int accolade_valide(char **cmd);
int getValeur_retour();
void free_debut_mots();

#endif