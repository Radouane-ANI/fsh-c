#include "header.h"

extern volatile sig_atomic_t signal_recu;
extern volatile sig_atomic_t signal_type;

int execute_cmd_externe(char **cmd)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }
    
    if (pid == 0) {
        restore_signal_for_child();
        if (redirection(cmd) == 1) {
            exit(1);
        }
        if (execvp(cmd[0], cmd) == -1) {
            perror("execvp");
            free_cmd(cmd);
            exit(1);
        }
    }
    
    int status;
    while (1) {
        pid_t w = waitpid(pid, &status, 0);
        if (w == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("waitpid");
            return 1;
        }
        break;
    }

    if (WIFSIGNALED(status)) {
        int sig = WTERMSIG(status);
        if (sig == SIGINT) {
            signal_recu = 1;
            signal_type = SIGINT;
            return 255;  
        }
        else if (sig == SIGTERM) {
            if (!signal_recu) {
                signal_recu = 1;
                signal_type = SIGTERM;
                return 255; 
            } else {
                signal_recu = 0; 
                signal_type = 0;
                return 0;
            }
        }
    }
    
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return 1;
}