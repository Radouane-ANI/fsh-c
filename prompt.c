#include "header.h"

char prompt[BUFF];

int setup_prompt(int valeur_retour)
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
        if (valeur_retour ==255) max_display_length =23
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
        return 0;
}