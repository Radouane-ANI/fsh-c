#include "header.h"

volatile sig_atomic_t signal_recu = 0; 
volatile sig_atomic_t signal_type = 0; 

static void sigint_handler(int sig)
{
    (void)sig;
    signal_recu = 1;
    signal_type = SIGINT; 
}

static void sigterm_ignore(int sig)
{
    (void)sig;
}

void setup_signals(void)
{
    struct sigaction sa_int;
    memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    struct sigaction sa_term;
    memset(&sa_term, 0, sizeof(sa_term));
    sa_term.sa_handler = sigterm_ignore;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    sigaction(SIGTERM, &sa_term, NULL);
}

void restore_signal_for_child(void)
{
    signal(SIGTERM, SIG_DFL);
    signal(SIGINT, SIG_DFL);
}