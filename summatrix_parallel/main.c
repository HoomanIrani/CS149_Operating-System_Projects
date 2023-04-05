/*
 * CS 149
 * Assignment 2
 * Professor William B Andreopoulos
 * Authors: Sina Kalantar, Houman Irani
 * Date: 03/06/2022
 */

#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE_LENGTHS 100

//Function to get the substring if the number has more than 1 digit involved
int getSubString(char line[], int a, int b) {

    char sub1[a + 1];
    memcpy(sub1, &line[b], a);
    sub1[a] = '\0';
    int x1 = atoi(sub1);

    return x1;

}

//read all the files
int readData(int n, int fd[][2], int argc, char *argv[], int col) {

    if (n == 0)
        return 0;

    int pid = fork();

    if (pid == 0) {

        //make recursion to run through all the text files
        readData(n - 1, fd, argc, argv, col);
        char line[MAX_LINE_LENGTHS] = {0};
        FILE *file = fopen(argv[n], "r");

        //if the file is incorrect then => print: Cannot open file
        if (!file) {

            printf("Cannot open file\n");

            exit(1);
        }

        int i, sum = 0;

        // Get each line until everything is read
        while (fgets(line, MAX_LINE_LENGTHS, file)) {

            int count = -1;

            // Iterating each character through inside the loop.
            for (i = 0; line[i] != '\0'; i++) {


                if ((line[i] >= '0') && (line[i] <= '9')) {    // Checking for numeric characters

                    count++;
                    if (count / col >= 1)
                        break;

                    else {

                        // handling negative number
                        if (line[i - 1] == '-') {

                            int a = 1;
                            int b = i;
                            while((line[i+1] >= '0') && (line[i+1] <= '9')) {

                                i++;
                                a++;
                            }

                            int y = getSubString(line, a, b);
                            printf("Warning - Value -%i found.\n", y);
                            continue;
                        }

                            //handling positive number
                        else {

                            int a = 1;
                            int b = i;
                            while((line[i+1] >= '0') && (line[i+1] <= '9')){
                                i++;
                                a++;
                            }
                            int x = getSubString(line, a, b);
                            sum += x;
                        }
                    }
                }
            }
        }

        //close the file statement
        fclose(file);

        //Use the pipe to transfer the array
        write(fd[n - 1][1], &sum, sizeof(sum));
        exit(0);
    }

    else {

        close(fd[n -1][1]);
        wait(NULL);
    }

    return 0;
}


// Main Function
int main(int argc, char *argv[]) {

    // Return error if number of arguments are incorrect. Needs at least one file name, min, and max as entry.
    if (argc < 3)
        return 1;

    char * c = argv[argc - 1];
    int col = atoi(c);

    //Create variables
    int num_files = argc - 2, fd[argc][2];

    //Create pipe for each file
    for (int i = 0; i < num_files; i++)
        pipe(fd[i]);

    readData(num_files, fd, argc, argv, col);

    //Read from pipe and store data in new_arr
    int sumMatrix = 0;

    //add up the sum of all text files
    for (int j = 0; j < num_files; j++) {

        int sumOfChild;
        read(fd[j][0], &sumOfChild, sizeof(sumOfChild));
        sumMatrix += sumOfChild;
    }

    //print out the total sum of the matrix
    printf("Total Sum: %d\n", sumMatrix);

    return 0;
}