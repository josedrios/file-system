/**************************************************************
 * Class::  CSC-415-01 Spring 2024
 * Name:: Gursimran Singh, Cole Brausen, Jose Rios
 * Student IDs:: 922759471, 918418987, 921160471
 * GitHub-Name:: guri408, cb-1881, Colorbomb1
 * Group-Name:: Team 3
 * Project:: Basic File System
 *
 * File:: fsInit.c
 *
 * Description:: Main driver for file system assignment.
 *
 * This file is where you will start and initialize your system
 *
 **************************************************************/


// Libraries
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "fsLow.h"
#include "mfs.h"
#include "VCB.h"
#include "FSM.h"
#include "DirectoryEntry.h"

// Forward declarations for the directory manipulation functions
int fs_mkdir(const char *pathname, mode_t mode);
int remove_directory(const char* path);
char* get_parent_filepath(const char* filepath);
char* get_child_filepath(const char* filepath);
void printDirectoryEntries(DirectoryEntry* entries);

int initFileSystem(uint64_t numberOfBlocks, uint64_t blockSize) {
    printf("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);

    VCB* volume = malloc(blockSize);
    if (volume == NULL) {
        fprintf(stderr, "Failed to allocate memory for the volume control block.\n");
        return -1;
    }
    printf("Size of VCB: %ld\n", sizeof(volume));

    // Initialize the partition system and read the volume
    read_volume(volume);

    // Check digital signatures
    if (check_signatures(volume)) {
        printf("SIGNATURES PASSED\n");
        write_free_space_map(volume->position_freespace, volume->blocks_freespace);

        // Read root directory
        read_root_directory(volume->root_dir_pos);
    } else {
        printf("SIGNATURES DID NOT PASS... MOVING ON...\n");
    }

    // Clear the volume structure in memory
    memset(volume, 0, blockSize);

    // Initialize the volume control block
    init_volume_control_block(volume, numberOfBlocks, blockSize);

    // Setup the bitmap for block management
    init_free_space_map(volume);

    // Initialize the root directory and assign its position in the VCB
    volume->root_dir_pos = init_root_directory(blockSize);

    // Write any changes to the volume
    write_volume(volume);

    // Free resources and clean up
    free(volume);
    volume = NULL;
    //exitFileSystem();
    return 0;
}

void exitFileSystem() {
    printf("System exiting\n");
    //cleanup();
}




