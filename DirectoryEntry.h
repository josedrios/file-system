/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Gursimran Singh, Cole Brausen, Jose Rios
* Student IDs:: 922759471, 918418987, 921160471
* GitHub-Name:: guri408, cb-1881, Colorbomb1
* Group-Name:: Team 3
* Project:: Basic File System
*
* File:: DirectoryEntry.h
*
* Description:: Header file for our directory, contains list
* of functions for the directory and the struct of the 
* directory
*
**************************************************************/

#ifndef DIRECTORY_ENTRY_H
#define DIRECTORY_ENTRY_H

// Libraries
#include "FSM.h"
#include "VCB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fsLow.h"

#define FS_TYPE_FILE 0
#define FS_TYPE_DIR 1
#define FS_TYPE_UNUSED -1
#define MAX_FILEPATH_SIZE 225
#define	MAX_FILENAME_SIZE 20
#define MAX_DIRECTORY_DEPTH 10
#define MAX_NESTED_DIRS 32
#define N_DIR_ENTRIES 20
#define MAX_PATH_LENGTH 4096

extern char *PWD;

// Directory entry structure for the file system
typedef struct {
    // Name of the file or directory
    char name[80];    

    // Flag indicating whether this is a directory (1) or a file (0) or unused (-1)   
    int type;      

    // Size of the file in bytes       
    int file_size;    

    // Starting location of the file on disk
    int starting_block;   

    // Number of blocks used by the file 
    int number_blocks; 

    // Time of last access 
    time_t time_accessed; 

    // Time of last modification
    time_t modified_time;  

    // Time of last status change     
    time_t time_created;    

    // Pointer to parent directory
    struct DirectoryEntry* parent;

    // Pointer to first child
    struct DirectoryEntry* children; 

    // Pointer to next sibling
    struct DirectoryEntry* next; 

} DirectoryEntry;

extern DirectoryEntry* root_dir;

extern DirectoryEntry* currentDir; 



uint64_t init_root_directory();
int64_t read_root_directory(uint64_t position);
DirectoryEntry * write_to_disk(DirectoryEntry *thisDirectory, DirectoryEntry *dirTable);

uint64_t init_directory_OLD(DirectoryEntry *parentDir, const char *name);
uint64_t init_directory(DirectoryEntry *parentDirTable, DirectoryEntry *set_as_parent, const char *name);

void setup_directory_entries(DirectoryEntry *dir, uint64_t selfBlock, uint64_t parentBlock);
DirectoryEntry* read_directory(DirectoryEntry* dir); 
void printDirectoryEntries(DirectoryEntry* entries);

int create_directory(const char* path);
int remove_directory(const char* path);

char* get_parent_filepath(const char* filepath);
char* get_child_filepath(const char* filepath);
char *all_path_to_abs_path(const char *pathname);
DirectoryEntry *lookup_path(const char *pathname);
DirectoryEntry* get_directory_entry(const char* pathname);
DirectoryEntry * lookup_name_in_dir(DirectoryEntry *dir, const char *name); 


DirectoryEntry * lookup_file(const char *pathname);
DirectoryEntry * lookup_dir(const char *pathname);

void test_all_path_to_abs_path();
void test_lookup_path();
#endif