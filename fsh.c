#include "header.h"

int valeur_retour = 0;
char **mots;

void free_debut_mots()
{
    free_cmd(mots);
}

int getValeur_retour()
{
    return valeur_retour;
}

int loop()
{
    valeur_retour = 0;
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

        mots = separer_chaine(ligne);
        free(ligne);

        if (mots[0] == NULL)
        {
            free_cmd(mots);
            continue;
        }

        valeur_retour = execute_cmd(mots);
        free_cmd(mots);
    }
}
