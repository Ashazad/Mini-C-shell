#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

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

// Function that each thread will run to execute a command
void *execute(void *args) {
    char **command = (char **)args;
    if (execvp(command[0], command) == -1) {
        perror("execvp");
    }
    exit(EXIT_FAILURE);
}

void shell_loop() {
    char *line;
    char **args;
    pthread_t thread;

    do {
        printf("> ");
        line = read_line();
        args = split_line(line);

        if (args[0] != NULL) {
            // Create a new thread for each command
            if (pthread_create(&thread, NULL, execute, (void *)args) != 0) {
                perror("pthread_create");
            } else {
                // Detach the thread to allow it to run independently
                pthread_detach(thread);
            }
        }

        free(line);
    } while (1);
}

int main() {
    shell_loop();
    return EXIT_SUCCESS;
}
