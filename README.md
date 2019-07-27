# light-daemon
This is a daemon program to execute a chosen CLI command in the background at set intervals.
Made by Thomas Hardern (https://github.com/FeynmanTesla), June 2019.
All work is in the public domain.

The process will detatch and run in the background, so even if you close the CLI you start the daemon in, it will continue running.

Make the program by cding into the light-deamon directory and running the "make" command. Run the program by executing the command "./daemon \<arguments>".

Arguments format: <dir to execute in (optional)> <execution time period (interval) in microseconds> <the command to repeatedly execute, as separate args where applicable>

Note that you can choose whether or not to wrap the command to execute in quotes. If not, the daemon programmatically builds up the command from multiple arguments itself.

To run the program (or any command) on startup on Linux, add commands calling the program to the /etc/rc.local file (on Linux), make a script calling it within the /etc/init.d/ directory and make it executable with "chmod +x", and add an entry to your crontab file with "sudo crontab -e" to open it and then adding "@reboot \<command>", where \<command> calls executable running the daemon. Use an absolute path in all instances to avoid errors.
