#include "header.h"

int loop()
{
    int valeur_retour = 0;
    setup_signals();
    rl_outstream = stderr;
    while (1)
    {
        if (setup_prompt(valeur_retour))
            return 1;
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
               if (mots[2] != NULL)
                {
                    int test = redirection(mots);
                    if (test == 0)
                    {
                        valeur_retour = 0;
                        free_cmd(mots);
                        free(ligne);
                        return valeur_retour;
                    }
                    else if (test == 1)
                    {
                        valeur_retour = 1;
                        free_cmd(mots);
                        free(ligne);
                        return valeur_retour;
                    }
                    write(2, "exit: too many arguments\n", 26);
                    valeur_retour = 1;
                    free_cmd(mots);
                    free(ligne);
                    continue;
                }
                else
                {
                    valeur_retour = atoi(mots[1]);
                }
            }
            free_cmd(mots);
            free(ligne);
            return valeur_retour;
        }
        valeur_retour = execute_cmd(mots);
        free_cmd(mots);
        free(ligne);
    }
}

