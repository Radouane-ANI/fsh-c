#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int redirection(char **cmd) {
    int fd;
    int i = 0;
    int count = 0;

    while (cmd[i] != NULL) {
        if (strcmp(cmd[i], ">") == 0) {
            count++;
            fd = open(cmd[i + 1], O_WRONLY | O_CREAT | O_EXCL, 0664);
            if (fd == -1) {
                perror("open");
                return 1;
            }
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("dup2");
                return 1;
            }
            close(fd);
        } else if (strcmp(cmd[i], ">|") == 0) {
            count++;
            fd = open(cmd[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0664);
            if (fd == -1) {
                perror("open");
                return 1;
            }
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("dup2");
                return 1;
            }
            close(fd);
        } else if (strcmp(cmd[i], ">>") == 0) {
            count++;
            fd = open(cmd[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0664);
            if (fd == -1) {
                perror("open");
                return 1;
            }
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("dup2");
                return 1;
            }
            close(fd);
        } else if (strcmp(cmd[i], "<") == 0) {
            count++;
            fd = open(cmd[i + 1], O_RDONLY);
            if (fd == -1) {
                perror("open");
                return 1;
            }
            if (dup2(fd, STDIN_FILENO) == -1) {
                perror("dup2");
                return 1;
            }
            close(fd);
        } else if (strcmp(cmd[i], "2>") == 0) {
            count++;
            fd = open(cmd[i + 1], O_WRONLY | O_CREAT | O_EXCL, 0664);
            if (fd == -1) {
                perror("open");
                return 1;
            }
            if (dup2(fd, STDERR_FILENO) == -1) {
                perror("dup2");
                return 1;
            }
            close(fd);
        } else if (strcmp(cmd[i], "2>|") == 0) {
            count++;
            fd = open(cmd[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0664);
            if (fd == -1) {
                perror("open");
                return 1;
            }
            if (dup2(fd, STDERR_FILENO) == -1) {
                perror("dup2");
                return 1;
            }
            close(fd);
        } else if (strcmp(cmd[i], "2>>") == 0) {
            count++;
            fd = open(cmd[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0664);
            if (fd == -1) {
                perror("open");
                return 1;
            }
            if (dup2(fd, STDERR_FILENO) == -1) {
                perror("dup2");
                return 1;
            }
            close(fd);
        } 
         else {
            i++;
            continue;
        }
        int j;
        for (j = i; cmd[j + 2] != NULL; j++) {
            cmd[j] = cmd[j + 2];
        }
        cmd[j] = cmd[j + 2];
        i--;
    }
    if (count == 0) {
        return 2;
    }
    return 0;
}
