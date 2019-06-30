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

int safecat(char *dest, const char *src) {
    strcat(dest, src);
    //TODO: implement error detection here; return -ve val if op fails
    return 1;
}

int main(int argc, char** argv) {
    // args format: executable path, local dir to sync to, syncing time period in seconds, 0 or more args to pass to grive
    if (argc < 3) debug("fewer than two args", 1); // minimum of 2 actual args - 1st is path to executable by convention
    
    int syncingTimePeriod = atoi(argv[2]);
    if (syncingTimePeriod == 0) debug("failed to convert syncing period string to int", 1); // failed to convert syncing period to an int

    // try to fork from parent process to make a new child process
    pid = fork();

    if (pid < 0) debug("couldn't fork", 1); // couldn't fork - fail
    if (pid > 0) exit(EXIT_SUCCESS); // forking worked but this process is still parent: parent has pid > 0, child has pid of 0
    // here pid == 0, so from here we know we are in the child process
    
    // umask needed here if making new files
    // open logs here?

    debug("checkpoint 31\n", 0);
    
    sid = setsid(); // try to move to new session
    if (sid < 0) debug("couldn't get new SID", 1); // failed to do so
    
    // try to go to the local directory where syncing will happen
    if ((chdir(argv[1])) < 0) debug("couldn't change cwd", 1); // exit if fails

    debug("checkpoint 37\n", 0);
    
    // closing standard fds
    if (close(STDIN_FILENO) != 0) debug ("stdin failed to close", 1);
    // if (close(STDERR_FILENO) != 0) debug ("stderr failed to close", 1); // maybe open this for debugging?
    // if (close(STDOUT_FILENO) != 0) debug ("stdout failed to close", 1); //TODO: fix bug here
    
    /* Daemon-specific initialization goes here */

    debug("checkpoint 44\n", 0);

    // build up grive command to repeatedly run from args here
    char command[1024];
    if (sprintf(command, "grive") < 0) debug("failed to initialise command string", 1); // initialising the command string
    for (int i = 2; i < argc; i++) {
        if (safecat(command, " ") < 0) debug("string concat failed on space", 1); // prepending each argument with a space character
        if (safecat(command, argv[i])) debug("string concat failed on arg", 1); // adding each argument relevent to the grieve command
    }
    
    debug("command is ", 0);
    debug(command, 0);
    debug("\n", 0);
    
    // main daemon loop
    while (1) { // runs until signal to exit is received
        // int syncStatus = system(command); TODO: handling?
        system(command);
        sleep(syncingTimePeriod); // wait for a bit before resyncing
    }

    return EXIT_SUCCESS;
}