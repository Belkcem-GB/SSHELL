# SSHELL
Authors: Belkacem, BEKKOUR
Emails: BBEKKOUR23@ubishops.ca, belkacem.bekkour@worldlearningalgeria.org

+++++ BelkacemShell SHELL ++++

Overview

sshell is a custom shell written in C. Most of the functionalities implemented by this shell are using the sytem calls and POSIX API. Basically, the shell will have a parent main process that represent the shell session and it's always activated (runing), and this is where the more and the exit commands are ran. The second process is a child of the main process and will be created to execute the EXTERNAL COMMANDS, where this child process will replace the parent image process to make sure that the shell session process will always be runing. The third process handle the background commands execution, where the parent process won't wait for the termination of the background process (10s). So the shell session will will keep runing untill the termination.

foreground commands are those that are executed in the foreground, meaning they are run synchronously, and the shell waits for their completion before allowing further input. Foreground commands are typically non-background commands that are not followed by an ampersand '&' character. In the code you provided, the following lines of code handle foreground commands. On the other hand, background commands are executed asynchronously, allowing the shell to continue accepting new input and running other commands without waiting for the background command to finish. Background commands are specified by appending an ampersand '&' character to the command, indicating that they should be run in the background. The code handles background commands by forking a child process to execute them, allowing the parent process to continue running the main loop without waiting for the background command to complete. The parent process can also track the status of background commands for later reporting.

Implementation Details
-------------------------------------------------------------------------------------------------------------------------------------------
- **Read configuration file (shconfig):** create a fuction "readConfig()" that open and read ("r") the configuration file using "fopen()", parse it  (if it exist) and then extract the HSIZE and the VSIZE paramters and close it using fclose(). If the file is does not exit create a new file using fopen(); ("w"), and wirite 2 lines using fprint(), "VSIZE 40" and "HSIZE 75" and close it at the end. If any problem occurs while creating the shconfig file, the "Error while creating shconfig: ERROR_MSG" will apear to the user using perror() function.
===========================================================================================================================================
- **main function:** in the main function we started calling the already created function "readConfig()". I have set an envirment variable that specify when the system should search for exucutable files using setenv() fucntion (we will need thiss to run the external commands). In order to make the shell session always open i have made all the code in an infinite while loop that can be broken using "break" (in case of calling "exit" cmd). The "command[1000]" of type char will hold the passed command, using the "fget()" function. Since the enter will be cosidred as a part of the command, i have replaced "\n" by "\0" in the array (null terminator).

-> In order to distinguish beteen the passed commands, i have used the contditional chain using "if" statements.
--------------------------------------------------------------------------------------------------------------------------------------------
--> If the command == "exit": print "byebye from CS564" and Break the infinite loop (end the sseion), also it will loop on all the background processes and kill them before breaking the loop.
--------------------------------------------------------------------------------------------------------------------------------------------
--> ELSE If command == "more": (compare only the first 4 elements of the command array). In order to extract the file name from the command, i have used strtok() function to split the cmd into words or (tokens) starting from the first entred space " " (after more). If the file name is missed, a message "Enter more <filename>" will be returned to the user. Otherwise we call the displayPBP() function.
--------------------------------------------------------------------------------------------------------------------------------------------
---> **displayPBP():** This function is used to display the content of the passed (.txt) file page by page, this is typically done by opening the passed file through the command using POSIX fopen(), checking if it's availible and if it's not, then the system will print a specific error message to the user. Otherwise if the file is already availible, i have created an array buffer called line[hsize+1], this array will hold the lines that should be passed to the user and it have hsize + 1 as a size for the null terminator "\0". For the purpose of reading pages interatevly, i have used a while loop the can be broken by pressing "ENTER", the while loop will keep reading the pages by respecting the passed (hsize, vsize) using fgets() POSIX, pass it to the buffer and then print it. Note that after printing every page, the system will ask the user whther to print the next page or break the loop. The file then will be closed using fclose() POSIX.

-->  ELSE IF the command is prefixed by "& " set isBackground=1. Note that by default the command is considred as a non-background cpmmand (isBackground=0). In addtion to that i have skipped the first 2 charactes of the command and copy the pure command to the command array variable using the strcpy() function.


--> IF isBackground = 0: if the variable isBackground hold a 0 value then the command will be treated as an EXTERNAL COMMAND. The external command should be runned in a different process, that's why i have created a new child using the fork() system call. If the child is properly created (pid == 0)we parse the command in order to extract the command and its arguments(I used chatGPT to parse this command), and then replace the current process image by the external command process using the system call execvp(args[0], args). Where args[0] hold the command and argrs: is an array that hold the command arguments. If anny occurs during the exection of the execvp() system call, "Execvp system call failed" message will apear with the appropriate error, and terminate the current child process with status of "1".

---> IF pid<0: this means that there is an error to fork a message "Failed to fork" will apear to the user with appropriate error message.

---> ELSE: here is the part of the parent process execution for the external commands. The parent process should wait for the termination of its child this will check the termination status of the child process.

-------------------------------------------------------------------
--> ELSE (Background command): if isBackground != 0 means isBackground = 1. So the entred command will be treated as a background command. Hence, we create a new child to run the background command, check if it's created properly and parse the command to extract the arguments and the command itself, and then going by the same steps as we did in runing the EXTERNAL COMMAND. However before calling the execvp() system call i have called the sleep(10) function in order to make the background command run in the background for 10s. 

--> In the part of the parent process of the background command, the parent process should't wait for the background process to finish. This will ensure making the shell session always activated and not delayed for any reason. In the same section,i have wrote some code to track the background process status. Hence, i ceated a struct that hold the PID and status of the background process. So each time a background process is created we hold its PID and its satus in an array called backgroundProcesses[], and // Function to display the status of background processess status the printBackgroundStatus() will print its status each time.

============================================================================================================================================

Test Suite Completeness

I believe my test suite is complete as it covers various aspects of the shell's functionality, including external command execution, file display, and background process handling. Additionally, we conducted edge case testing to handle scenarios where command inputs may vary in complexity.

Here are some test commands followed by there output:

belkainelec@belkainelec:~/Desktop/Assignment/ASS1-TESTS/Report1$ make clean
rm -f sshell
belkainelec@belkainelec:~/Desktop/Assignment/ASS1-TESTS/Report1$ make
gcc -Wall -Wextra -o sshell sshell.c
belkainelec@belkainelec:~/Desktop/Assignment/ASS1-TESTS/Report1$ make run
./sshell
sshell> la
Execvp system call failed: No such file or directory
sshell> ls
makefile  myfile.txt  README  shconfig	sshell	sshell.c
sshell> more
Enter more <filename>
sshell> more myfile.txt
This is line 1 of myfile.txt.
This is line 2 of myfile.txt.
This is line 3 of myfile.txt.
This is line 4 of myfile.txt.
This is line 5 of myfile.txt.
This is line 6 of myfile.txt.
This is line 7 of myfile.txt.
This is line 8 of myfile.txt.
This is line 9 of myfile.txt.
This is line 10 of myfile.txt.
This is line 11 of myfile.txt.
This is line 12 of myfile.txt.
This is line 13 of myfile.txt.
This is line 14 of myfile.txt.
This is line 15 of myfile.txt.
This is line 16 of myfile.txt.
This is line 17 of myfile.txt.
This is line 18 of myfile.txt.
This is line 19 of myfile.txt.
This is line 20 of myfile.txt.
This is line 1 of myfile.txt.
This is line 2 of myfile.txt.
This is line 3 of myfile.txt.
This is line 4 of myfile.txt.
This is line 5 of myfile.txt.
This is line 6 of myfile.txt.
This is line 7 of myfile.txt.
This is line 8 of myfile.txt.
This is line 9 of myfile.txt.
This is line 10 of myfile.txt.
This is line 11 of myfile.txt.
This is line 12 of myfile.txt.
This is line 13 of myfile.txt.
This is line 14 of myfile.txt.
This is line 15 of myfile.txt.
This is line 16 of myfile.txt.
This is line 17 of myfile.txt.
This is line 18 of myfile.txt.
This is line 19 of myfile.txt.
This is line 20 of myfile.txt.
Press (enter) to continue || Any key + enter to finish: r
sshell> sshell> & pwd
sshell> ls
makefile  myfile.txt  README  shconfig	sshell	sshell.c
sshell> ls
makefile  myfile.txt  README  shconfig	sshell	sshell.c
sshell> /home/belkainelec/Desktop/Assignment/ASS1-TESTS/Report1
lw
Execvp system call failed: No such file or directory
sshell> & ls
sshell> more myfile.txt
This is line 1 of myfile.txt.
This is line 2 of myfile.txt.
This is line 3 of myfile.txt.
This is line 4 of myfile.txt.
This is line 5 of myfile.txt.
This is line 6 of myfile.txt.
This is line 7 of myfile.txt.
This is line 8 of myfile.txt.
This is line 9 of myfile.txt.
This is line 10 of myfile.txt.
This is line 11 of myfile.txt.
This is line 12 of myfile.txt.
This is line 13 of myfile.txt.
This is line 14 of myfile.txt.
This is line 15 of myfile.txt.
This is line 16 of myfile.txt.
This is line 17 of myfile.txt.
This is line 18 of myfile.txt.
This is line 19 of myfile.txt.
This is line 20 of myfile.txt.
Press (enter) to continue || Any key + enter to finish: makefile  myfile.txt  README  shconfig	sshell	sshell.c
d
sshell> sshell> & pwd
sshell> exit
byebye from CS564!

===========================================================================================================================================
make clean
ll
submit ~/sshell/
result: DONE!

sbmit your work to 
scp -r sshell/ linux.ubishops.ca
===========================================================================================================================================
REFERENCES:
Course labs/PPT of Dr Namane Rachid EE426.
Course Handout of Dr Stefan Bruda CS546.
ChatGPT. (Parse the external command line).
And I have read many stack overflow resources, in order to get this final code.

Thank you!
Best Regards.
