/**
 * Daemon program to execute a chosen command in the background at set intervals.
 * Made by Thomas Hardern (https://github.com/FeynmanTesla), June 2019.
 * All work in the public domain.
 */

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pid_t pid, sid;

void debug(char* msg, int toExit) {
    printf("%s\n", msg);
    if (toExit == 1) exit(EXIT_FAILURE);
}

void loop(char* command, int interval) {
    while (1) { // runs until signal to exit is received
        system(command);
        sleep(interval); // wait for a bit before resyncing
    }
}

char* buildCommand(char** argv, int argc, int startArgIndex) {
    if (argc - startArgIndex < 1) debug("build command given invalid parameters", 1);

    char command[1024]; //TODO: proper mallocing
    for (int i = startArgIndex; i < argc; i++) { // add each argument to the command string
        strcat(command, " "); // prepending each argument with a space character
        strcat(command, argv[i]); // adding each argument relevent to the grieve command
    }

    return command;
}

void setup(char* command, int period) {
    // try to fork from parent process to make a new child process
    pid = fork();

    if (pid < 0) debug("couldn't fork", 1); // couldn't fork - fail
    if (pid > 0) exit(EXIT_SUCCESS); // forking worked but this process is still parent: parent has pid > 0, child has pid of 0
    // here pid == 0, so from here we know we are in the child process
    
    sid = setsid(); // try to move to new session
    if (sid < 0) debug("couldn't get new SID", 1); // failed to do so
    
    // closing standard fds
    if (close(STDIN_FILENO) != 0) debug("stdin failed to close", 1);
    // if (close(STDERR_FILENO) != 0) debug("stderr failed to close", 1); // maybe open this for debugging?
    if (close(STDOUT_FILENO) != 0) debug("stdout failed to close", 1);

    // build up grive command to repeatedly run from args here
    
    
    debug("command is ", 0);
    debug(command, 0);
    debug("\n", 0);
    
    loop(command, period); // main daemon loop
}


int main(int argc, char** argv) {
    // args format: [path to this executable] <dir to execute in (optional)> <syncing time period in seconds> <the command, as separate args where applicable>
    if (argc < 3) debug("fewer than two args", 1); // minimum of 2 actual args - 1st is path to executable by convention

    /*TODO:
    if first arg is a number then take that as the period; no exe dir given
    else, try to find a number another along and get a exe dir
        if there are at least 3 args and the second arg parses as a number 
            then take first as exe path, second as period, rest as commmand
            try to move into exe path taken
        else then fail
     */

    int period = atoi(argv[1]);
    if (period == 0) { // failed to get period from first arg
        if (argc > 3 && (period = atoi(argv[2])) != 0) { // if enough args to have exe dir and period parses from 2nd arg
            if ((chdir(argv[1])) < 0) debug("couldn't change cwd", 1); // try to move into the desired execution directory
        }

        else debug("args of incorrect format to include an execution directory path", 1);
    } else { // exe path not given

    }
    
    // try to go to the local directory where syncing will happen

    
    return EXIT_SUCCESS;
}