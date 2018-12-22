# UNIX Shell

## Execution
```
make
./shell
make clean
```

## Features

- Shell prompt is displayed as <username@hostname:PWD>. PWD is displayed relative to HOME directory, which is set as the directory in which the shell is initially started.
- Semi-colon separated list of commands
- Input/Output Redirection & Piping
- Background/Foreground processes
- Signal Handling

## Commands implemented:
- pwd : returns present working directory
- cd [PATH] : changes current working directory to the specified directory, if it exists. Otherwise, error is shown
- echo : Displays a string on the console.
- ls [OPTIONS] [PATH] : displays information about the directory specified in PATH, if it exists, or about the PWD.  
Options:
  - -l : displays detailed information
  - -a : displays hidden files
- pinfo [PID] : displays process information about the shell's process, or PID if specified
- setenv [VAR] [value] : Sets value of environment variable VAR to value. If variable does not exist, it is created and initialised.
- unsetenv [VAR] : destroys the environment variable VAR
- jobs : prints a list of all currently running jobs
- kjob [jobNumber] [signalNumber] : takes the job id of a running job and sends a signal value to that process
- fg [jobNumber] : brings a running or a stopped background job with given job number to foreground.
- bg [jobNumber] : changes a stopped background job to a running background job.
- overkill : kills all background process at once.
- quit : exits the shell
- All other commands can be executed by forking parent process.



* Developed by: Yashaswi Pathak (20161149)