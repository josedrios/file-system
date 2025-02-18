/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Jose Rios
* Student ID:: 921160471
* GitHub-Name:: Colorbomb1
* Project:: Assignment 6 – Device Driver
*
* File:: driver.c
*
* Description:: This file contains the testing functionality
* to utilize my percentage grade to letter grade converter
*
**************************************************************/

// Libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// Sending and receiving message size
#define BLOCK_SIZE 512

// Receiver to retreive message from driver
static char receiver[BLOCK_SIZE]; 


int main (int argc, char* argv[]) {
    int fd, info;
    int ret;
    long n1, n2, n3;
    char Message[] = {"My Message That Send"};

    // Sender to send input from this test program
    char* sender [512];
    sender[0] = '\0';

    // Open the driver
    fd = open("/dev/driver", O_RDWR);
    printf("returned from open file, %d\n", fd);

    // Opening device driver response
    if(fd < 0) {
        printf("Device Open Error\n");
        perror("Device File Open Error");
        return -1;
    }
    else 
    {
        printf("Device Open Success.\n");
    }

    // Getting user input
    while(1){
        // Prompting user for recommended input
        printf("Enter your grade(1-100) to see your letter grade (enter exit to exit): ");
        
        // Gathering inputted information
        scanf("%s", sender); 

        // If user entered "exit", leave the program
        if (strcmp(sender, "exit") == 0){
            return 0;
        }

        // Check if the input was valid
        int len = strlen(sender);
        int input = atoi(sender);
        if(input > 0 && input < 101){
            // If input was valid, leave the request input loop
            break;
        }
    }
    
    // WRITING TO DEVICE PROCESS
    printf("Writing your grade percentage of '%s' to device\n", sender);
    ret = write(fd, sender, BLOCK_SIZE);

    // CHECK IF MESSAGE WAS WRITTEN TO DEVICE
    if(ret < 0){
        printf("Failed to write message to device\n");
        return errno;
    }

    // Utilizing ioctl function to see what bytes were copied
    n2 = ioctl(fd, 3, &info);

    // Reading from device
    printf("Reading from device...\n");
    ret = read(fd, receiver, BLOCK_SIZE);

    // Display output
    printf("Output: %s\n", receiver);

    // Close driver after utilization is done
    close(fd);

    return 0;
}
