/**
* Author: Houman Irani
* Class: CS 149
* Assignment 3
* Name of the assignment: proc_manager.c
* Professor William Andreopoulos
*
* Description:
* In this assignment my program reads parameters and commands necessary for execution from the input files given,
* then it starts and waits for the processes to complete and it writes standard output and standard error
* into the log files.
*
**/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


//write_err function for writing the errors
int write_err (char *error1) {

    char num1 = '\n\n';
    write(2, "Error:\n", strlen("Error:\n"));
    write(2, error1, strlen(error1));
    write(2, &num1, 1);

    return 0;
}


// Main Function
int main(int argc, char *argv[]) {

    //Variables

    int fdOut, fdError;
    int number = 0;

    char fOut[15] = "", fError[15] = "";
    char *error;
    char inputs[256] = "";
    char *argument[16] = {NULL};
    char *readLines;

    pid_t pid, cpid;
    int numOfChildren = 0, waitStatus = 0, errorFlag;


    FILE *my_file = fopen(argv[1], "r");


    if (my_file == NULL) {

        error = strerror(errno);
        write_err(error);
    }


    else {
        //Read all the lines

        while (1) {

            readLines = fgets(inputs, sizeof(inputs), my_file);
            printf("%s", readLines);

            if (readLines == NULL)

                break;
            number++;
            int j = 0;
            char *c;


            argument[0] = strtok(inputs, " \t\n");

            if (argument[0] != NULL)

                while ((c = strtok(NULL, " \t\n")) != NULL) {

                    j++;
                    argument[j] = c;
                }



            pid = fork();

            //Error when PID = -1
            if (pid == -1) {

                error = strerror(errno);
                write_err(error);
            }


            else {

                if (pid == 0) {

                    sprintf(fOut, "%d.out", getpid());
                    fdOut = open(fOut, O_RDWR | O_CREAT | O_APPEND);
                    fchmod(fdOut, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

                    if (fdOut != -1)

                        dup2(fdOut, 1);
                    printf("Starting command %d: child %d pid of parent %d\n", number, getpid(), getppid());


                    sprintf(fError, "%d.err", getpid());
                    fdError = open(fError, O_RDWR | O_CREAT | O_APPEND);
                    fchmod(fdError, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);



                    if (fdError != -1) {

                        dup2(fdError, 2);

                    }

                    errorFlag = execvp(argument[0], argument);


                    if (errorFlag) {

                        char err_string[64] = "";
                        int exitCode = errno;
                        snprintf(err_string, sizeof(err_string),
                                 "Failed to execute the command: %s err: %d\n", argument[0], exitCode);
                        perror(err_string);
                        exit(exitCode);
                    }

                    //close the file
                    close(fdOut);
                    close(fdError);

                    break;
                }

                else {

                    numOfChildren++;
                }

            }

        }

    }


    if (my_file != NULL) {

        fclose(my_file);
    }

    if (pid != 0) {

        while (numOfChildren > 0) {

            cpid = wait(&waitStatus);

            char string_buffer[100] = "";
            char exitCode;

            int bb;
            int sig;


            sprintf(fOut, "%d.out", cpid);
            fdOut = open(fOut, O_RDWR | O_CREAT | O_APPEND);
            fchmod(fdOut, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);


            bb = snprintf(string_buffer, sizeof(string_buffer),
                         "Finished child %d pid of parent %d\n", cpid, getpid());
            write(fdOut, string_buffer, bb);


            sprintf(fError, "%d.err", cpid);
            fdError = open(fError, O_RDWR | O_CREAT | O_APPEND);
            fchmod(fdError, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);


            if (WIFEXITED(waitStatus)) {

                exitCode = WEXITSTATUS(waitStatus);
                bb = snprintf(string_buffer, sizeof(string_buffer),
                             "Exited with exitcode = %d\n", exitCode);
            }


            if (WIFSIGNALED(waitStatus)) {
                sig = WTERMSIG(waitStatus);
                bb = snprintf(string_buffer, sizeof(string_buffer),
                             "Killed with signal %d\n", sig);
            }


            write(fdError, string_buffer, bb);
            numOfChildren--;

        }

    }

    return 0;

}