#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define DELIM " \t\r\n\a"

char *read_line() {
    char *line = malloc(MAX_INPUT);
    if (fgets(line, MAX_INPUT, stdin) == NULL) {
        if (feof(stdin)) exit(0);
        else { perror("read_line"); exit(1); }
    }
    return line;
}

char **split_line(char *line) {
    int bufsize = MAX_ARGS, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    token = strtok(line, DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += MAX_ARGS;
            tokens = realloc(tokens, bufsize * sizeof(char*));
        }

        token = strtok(NULL, DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int execute(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("execvp");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

void shell_loop() {
    char *line;
    char **args;

    do {
        printf("> ");
        line = read_line();
        args = split_line(line);
        if (args[0] != NULL) {
            execute(args);
        }
        free(line);
        free(args);
    } while (1);
}

int main() {
    shell_loop();
    return EXIT_SUCCESS;
}
