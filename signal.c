#include "header.h"

volatile sig_atomic_t signal_recu = 0;

void ignore_signal(int sig)
{
    signal_recu = 1;
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