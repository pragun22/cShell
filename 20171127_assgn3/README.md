## RUNNING

- run "make"

- execute by using "./cShell"

- type "exit" to exit the shell

## FEATURES

#### ls command
###### Usage:
- ls takes arguments as flags (in the form -(flagname)) and directories (in the form -(directory name)) in any order.
- It lists the files of all the directories mentioned, in the form of option selected by the flags.
- If there is no directory mentioned, it lists the files of current directory

* "ls" - lists in alphabetical order
* "ls -a" - display hidden files
* "ls -l" - long list
* "ls -al"/"ls -la"/"ls -l -a"/"ls -a -l" - long list including hidden files


#### cd command
* "cd (absolute path)" - goes to absolute path
* "cd" - goes to ~/
* "cd .." - goes to previous directory
* "cd ." - goes to same directory
* "cd ~" goes to ~/

#### echo command
* "echo" - prints empty line
* "echo text" - prints "text"
* "echo "text" " - prints "text"

#### pwd command
* "pwd" - prints absolute path of present working directory

#### pinfo command
* "pinfo" : prints the process related info of the shell program
* "pinfo <pid>" : prints the process info about given pid
- pid --
- Process status -- {R/S/S+/Z}
- Memory
- Executable path -- (Absolute path - includes ~/ if executable is present inside the shell directory)

#### remind me

* usage: remindme tm "message"
* it will display the message in time tm


#### clock
* ‘clock’ command which displays dynamic date and time in your shell.
* usage- clock -t tm -n duration
* the command will only work for the given duration and then stop


#### background and foreground processes
* all bash commands works in background and foreground
* background processes ids are shown when they exit
