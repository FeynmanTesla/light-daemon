/**
 * Daemon program to execute a chosen CLI command in the background at set intervals.
 * Made by Thomas Hardern (https://github.com/FeynmanTesla), June 2019.
 * All work is in the public domain.
 */

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pid_t pid, sid;
static const int maxCommandLength = 1024;

/**
 * Print an error message and then exit the application with an exit code signifying an error.
 * Used for user feedback in arguments format before the main loop since file descriptors are closed before this.
 * @param msg the error message to print before exiting the application.
 */
void errorMessage(char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

/**
 * The main loop of the daemon. Executes a CLI command at an interval (i.e. time period).
 * @param command the command to repeatedly execute.
 * @param interval the time period between each command execution, in microseconds.
 */
void loop(char* command, int interval) {
    while (1) { // runs until signal to exit is received
        system(command); // execute the command
        usleep(interval); // wait for the length of the period between each execution
    }
}

/**
 * Build up the command to exeute into a single string from multiple given arguments.
 * @param argv the given arguments.
 * @param argc the number of given arguments.
 * @param startArgIndex the index of the first given argument to be in the command to execute (i.e. being built).
 */
char* buildCommand(char** argv, int argc, int startArgIndex) {
    if (startArgIndex < 0 || startArgIndex > argc - 1) errorMessage("build command given invalid parameters"); // note argc is already validated in main() before this
    char* command = (char*) malloc(sizeof(char) * maxCommandLength);

    for (int i = startArgIndex; i < argc; i++) { // add each argument to the command string
        if (i != startArgIndex) strcat(command, " "); // prepending each argument with a space character
        strcat(command, argv[i]); // adding each argument relevent to the command
    }

    return command;
}

/**
 * Preparing to run the daemon. Move to a new process and session to prevent becoming an orphan, etc.
 * Close file descriptors since the daemon is a background process so we don't want any user I/O.
 * @param command the command to repeatedly execute.
 * @param period the time in microseconds between each execution of the command.
 */
void init(char* command, int period) {
    pid = fork(); // try to fork from parent process to make a new child process

    if (pid < 0) errorMessage("couldn't fork");
    if (pid > 0) exit(EXIT_SUCCESS); // forking worked but this process is the parent process: parent has pid > 0, child has pid of 0
    // here pid == 0, so from here we know we are in the child process

    sid = setsid(); // try to move to new session
    if (sid < 0) errorMessage("couldn't get new SID"); // failed to do so
    // from here, we have moved to a new session

    // closing standard fds
    if (close(STDIN_FILENO) != 0) errorMessage("stdin failed to close");
    if (close(STDERR_FILENO) != 0) errorMessage("stderr failed to close");
    if (close(STDOUT_FILENO) != 0) errorMessage("stdout failed to close");

    loop(command, period); // enter the main daemon loop
}

/**
 * The main method and entry point into the daemon. Valiate and parse the given arguments, build up the command, and start executing it.
 * @param argc the number of given arguments. Note by convention the first argument is the path to this executable and is always present.
 * @param argv the given arguments. format: [path to this executable] <dir to execute in (optional)> <syncing time period in microseconds> <the command, as separate args where applicable>.
 */
int main(int argc, char** argv) {
    if (argc < 3) errorMessage("fewer than two args"); // minimum of 2 actual args - 1st is path to executable by convention

    int period = atoi(argv[1]);
    int firstCommandArgIndex = 2;

    if (period == 0) { // failed to get period (an int) from first arg, so assume execution dir is given
        if (argc < 4) errorMessage("first arg couldn't parse as an int and the number of args is too small to include an execution directory path");
        period = atoi(argv[2]);
        if (period == 0) errorMessage("the second argument couldn't parse as an int");
        if ((chdir(argv[1])) < 0) errorMessage("couldn't change cwd"); // try to move into the desired execution directory
        firstCommandArgIndex++;
    }

    char* command = buildCommand(argv, argc, firstCommandArgIndex);
    init(command, period);
    free(command);
    return EXIT_SUCCESS;
}