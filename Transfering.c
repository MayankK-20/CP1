#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 1024  // Max input size

// Function prototypes
void shell_loop();
char* read_user_input();
int parse_and_execute(char* input);
int launch(char **args);
int execute_piped_command(char **args1, char **args2);

int main() {
    // Start the shell loop
    shell_loop();
    return 0;
}

void shell_loop() {
    char* input;
    int status;

    do {
        printf("iiitd@possum:~$ ");  // Shell prompt
        input = read_user_input();  // Get user input
        status = parse_and_execute(input);  // Parse and execute command
        free(input);  // Free the allocated memory
    } while (status);  // Continue until user exits the shell
}

// Function to read user input
char* read_user_input() {
    char* input = NULL;
    size_t bufsize = 0;
    getline(&input, &bufsize, stdin);  // Read line of input
    return input;
}

// Function to parse and execute the command
int parse_and_execute(char* input) {
    char* args[MAX_LINE / 2 + 1];  // To hold command arguments
    char* token;
    int i = 0;

    // Check for piping (`|`)
    char* piped_args[2];
    char* pipe_token = strtok(input, "|");

    // No pipe detected, normal command execution
    while (pipe_token != NULL) {
        piped_args[i++] = pipe_token;
        pipe_token = strtok(NULL, "|");
    }

    if (i == 1) {
        // No pipe, parse and execute the single command
        token = strtok(piped_args[0], " \t\n");
        i = 0;
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        args[i] = NULL;

        return launch(args);
    } else if (i == 2) {
        // Pipe detected, handle piped commands
        char* args1[MAX_LINE / 2 + 1];
        char* args2[MAX_LINE / 2 + 1];

        // Parse first command
        token = strtok(piped_args[0], " \t\n");
        i = 0;
        while (token != NULL) {
            args1[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        args1[i] = NULL;

        // Parse second command
        token = strtok(piped_args[1], " \t\n");
        i = 0;
        while (token != NULL) {
            args2[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        args2[i] = NULL;

        return execute_piped_command(args1, args2);
    }

    return 1;  // Continue shell
}

// Launch a command without piping
int launch(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("Command execution failed");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("Fork failed");
    } else {
        // Parent process
        wait(NULL);  // Wait for the child to finish
    }
    return 1;  // Continue shell
}

// Function to execute two piped commands
int execute_piped_command(char **args1, char **args2) {
    int fd[2];  // File descriptors for pipe
    pid_t pid1, pid2;

    if (pipe(fd) == -1) {
        perror("Pipe failed");
        return 1;  // Continue shell
    }

    pid1 = fork();
    if (pid1 == 0) {
        // First child: executes the first command
        close(fd[0]);  // Close unused read end
        dup2(fd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(fd[1]);  // Close pipe write end
        if (execvp(args1[0], args1) == -1) {
            perror("Command 1 execution failed");
        }
        exit(EXIT_FAILURE);
    } else if (pid1 < 0) {
        // Forking failed
        perror("Fork failed");
        return 1;
    }

    pid2 = fork();
    if (pid2 == 0) {
        // Second child: executes the second command
        close(fd[1]);  // Close unused write end
        dup2(fd[0], STDIN_FILENO);  // Redirect stdin to pipe
        close(fd[0]);  // Close pipe read end
        if (execvp(args2[0], args2) == -1) {
            perror("Command 2 execution failed");
        }
        exit(EXIT_FAILURE);
    } else if (pid2 < 0) {
        // Forking failed
        perror("Fork failed");
        return 1;
    }

    // Parent process
    close(fd[0]);  // Close both ends of the pipe
    close(fd[1]);
    waitpid(pid1, NULL, 0);  // Wait for first child
    waitpid(pid2, NULL, 0);  // Wait for second child

    return 1;  // Continue shell
}
