#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 1024

char* read_user_input() {
    char* buffer = malloc(sizeof(char) * BUFFER_SIZE);
    if (buffer == NULL) {
        perror("Allocation error");
        exit(EXIT_FAILURE);
    }

    if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
        perror("fgets error");
        exit(EXIT_FAILURE);
    }

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }

    return buffer;
}

int create_process_and_run(char* command) {
    pid_t pid, wpid;
    int status;

    char* argv[64];
    char* token;
    int i = 0;

    token = strtok(command, " ");
    while (token != NULL) {
        argv[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    argv[i] = NULL;

    pid = fork();

    if (pid == 0) {
        if (execvp(argv[0], argv) == -1) {
            perror("execvp error");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork error");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int launch(char* command) {
    int status;
    status = create_process_and_run(command);
    return status;
}

void shell_loop() {
    int status;
    do {
        printf("iiitd@possum:~$ ");
        char* command = read_user_input();
        status = launch(command);
        free(command);  // Free the allocated memory after each command
    } while (status);
}

int main() {
    // Start the shell loop
    shell_loop();

    return 0;
}
