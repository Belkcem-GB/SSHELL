#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


#define CONFIG_FILE "shconfig"

char VSIZE_str[10] = "40";
char HSIZE_str[10] = "75";

// define the PATH env var
char ENVAR_PATH[] = "/bin:/usr/bin";


typedef struct {
    pid_t pid;
    int status; // 0= Running, 1= Completed
} BackgroundProcess;

BackgroundProcess backgroundProcesses[10];

int numBackgroundProcesses = 0; //runing processes

void readConfig() {
    // set the PATH environment variable
    setenv("PATH", ENVAR_PATH, 1);

    FILE *configFile = fopen(CONFIG_FILE, "r");
    if (configFile != NULL) {
        char line[100];
        while (fgets(line, sizeof(line), configFile)) {
            char variable[10];
            char value[10];
            if (sscanf(line, "%9s %9s", variable, value) == 2) {
                if (strcmp(variable, "VSIZE") == 0) {
                    strncpy(VSIZE_str, value, sizeof(VSIZE_str));
                } else if (strcmp(variable, "HSIZE") == 0) {
                    strncpy(HSIZE_str, value, sizeof(HSIZE_str));
                }
            }
        }
        fclose(configFile);
    } else {
        // create the CONFIG_FILE with default values IF DNE
        configFile = fopen(CONFIG_FILE, "w");
        if (configFile != NULL) {
            fprintf(configFile, "VSIZE 40\nHSIZE 75\n");
            fclose(configFile);
        } else {
            perror("Error while creating shconfig");
        }
    }
}


void displayPBP(const char *filename, int vsize, int hsize) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error while opening the file");
        return;
    }

    char line[hsize + 1]; // +1 for null terminator
    int lineCount = 0;

    //pid_t moreCommandPid = getpid(); // Get the process ID of the "more" command

    //printf("Process ID of 'more' command: %d\n", moreCommandPid);

    while (fgets(line, sizeof(line), file)) {
        // Truncate lines longer than hsize
        if (strlen(line) > (size_t)hsize) {
            line[hsize] = '\0';
        }
        printf("%s", line);

        lineCount++;
        if (lineCount >= vsize) {
            printf("Press (enter) to continue || Any key + enter to finish: ");
            char input[2];
            fgets(input, sizeof(input), stdin);
            if (input[0] != '\n') {
                break;
            }
            lineCount = 0;
        }
    }

    fclose(file);
}

void printBackgroundStatus() {
    int i;
    for (i = 0; i < numBackgroundProcesses; i++) {
        if (backgroundProcesses[i].status == 1) {
            printf("Background process with PID %d has completed.\n", backgroundProcesses[i].pid);
            
        }
    }
}

int main() {
    readConfig();

    while (1) {
        // display the prompt
        printf("sshell> ");

        char command[1000];
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = '\0';

        // check if the cmd is exit
        if (strcmp(command, "exit") == 0) {
                    for (int i = 0; i < numBackgroundProcesses; i++) {
                kill(backgroundProcesses[i].pid, SIGTERM);
            }
            // Print the status of background processes before exiting
            printBackgroundStatus();
            printf("byebye from CS564!\n");
            break;
        } else if (strncmp(command, "more", 4) == 0) {
            /// parse cmd to get the filename
            char *filename = strtok(command + 4, " ");
            if (filename != NULL) {
                // Display the file page by page due to VSIZE & HSIZE
                displayPBP(filename, atoi(VSIZE_str), atoi(HSIZE_str));
            } else {
                printf("Enter more <filename>\n");
            }
        } else {
            // check if the cmd is in the background (& )
            int isBackground = 0; // Set the cmd as non-background by default
            if (strlen(command) > 2 && command[0] == '&' && command[1] == ' ') {
                isBackground = 1;
                strcpy(command, command + 2);

                // printf("%s\n", command);
            }

            // external commands
            if (!isBackground) {
            
                int pid = fork();
                if (pid == 0) {
                    
                    char *args[100];
                    // Parse and set the arguments for the external command
                    args[0] = strtok(command, " "); // Command name
                    int argIndex = 1;
                    char *token = strtok(NULL, " ");
                    while (token != NULL) {
                        args[argIndex] = token;
                        token = strtok(NULL, " ");
                        argIndex++;
                    }
                    args[argIndex] = NULL;
                    execvp(args[0], args);
                    perror("Execvp system call failed");
                    exit(1);
                } else if (pid < 0) {
                    perror("Failed to fork");
                } else {
                    // this is the parent process for the external commands
                    
                    waitpid(pid, NULL, 0);
                    
                }
            } else {
            
                // background commands
                pid_t pidd = fork();
                
                if (pidd == 0) {
                    
                    char *args[100];
                    
                    args[0] = strtok(command, " ");
                    int argIndex = 1;
                    char *token = strtok(NULL, " ");
                    while (token != NULL) {
                        args[argIndex] = token;
                        token = strtok(NULL, " ");
                        argIndex++;
                    }
                    args[argIndex] = NULL;
                    sleep(10);

                    execvp(args[0], args);
                    perror("Execvp system call failed");
                    exit(1);
                } else if (pidd < 0) {
                    perror("Fork failed");
                } else {
                isBackground = 0;
                //sleep(1);
                    // parent process for background command
                    backgroundProcesses[numBackgroundProcesses].pid = pidd;
                    backgroundProcesses[numBackgroundProcesses].status = 0;
                    numBackgroundProcesses++;
                }
            }
        }
    }
    return 0;
}

