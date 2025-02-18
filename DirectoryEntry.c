/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Gursimran Singh, Cole Brausen, Jose Rios
* Student IDs:: 922759471, 918418987, 921160471
* GitHub-Name:: guri408, cb-1881, Colorbomb1
* Group-Name:: Team 3
* Project:: Basic File System
*
* File:: DirectoryEntry.c
*
* Description:: Contains functions related to our directory
* entry such as initializing and reading a directory 
*
**************************************************************/

// Libraries
#include "DirectoryEntry.h"
#include "fsLow.h"
#include "FSM.h"
#include "mfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>

// Globals

//rootdir global
DirectoryEntry* root_dir = NULL; 
DirectoryEntry root_dir_entry; 

// A WORK IN PROGRESS  

// Table row for a directory control block to manage a directory.
// This ensures a directory is only in memory in one place.
typedef struct b_dcb {
    int startingBlock; // serves as unused (-1) or LB position 1+
    DirectoryEntry * thisDirectory; // DirectoryEntry for this directory
    DirectoryEntry * dirTable; // Pointer to the first entry _in_ the directory
    int index;
} b_dcb;

#define MAXDCBS 20
b_dcb dcbArray[MAXDCBS];


// Initial a table to hold the directory control blocks
void initialize_dcbArray() {

    // Set all the entries to -1 (not used)
    for (int i=0; i<MAXDCBS; i++) {
        dcbArray[i].startingBlock = -1;
        dcbArray[i].index = i;
    }

    // Claim the first entry
    dcbArray[0].startingBlock = 0;
}


// Find the dcb for a given starting block (which is unique for any given directory)
b_dcb *find_dcb_for_starting_block(int startingBlock) {

    // Search through directory control block 
    for(int i=0; i<MAXDCBS; i++) {

        // Return found DCB index
        if (dcbArray[i].startingBlock == startingBlock) {
            return &dcbArray[i];
        }

    }

    // Dcb not found, return NULL
    return NULL;
}


// Linear search to find a free spot in the dcbArray
b_dcb *find_free_dcb() {

    // Search through directory control block
    for(int i=0; i<MAXDCBS; i++) {

        // Return respective index
        if (dcbArray[i].startingBlock == -1) {
            return &dcbArray[i];
        }
    }

    // If no free spots, return NULL
    return NULL;
}

// Find an empty directory entry in a directory table
int find_empty_spot(DirectoryEntry* dir) {

    int num_dir_entries = dir->file_size / sizeof(DirectoryEntry);

    // Search through directory table
    for(int i = 0; i<num_dir_entries;i++) {
        
        // If spot is empty, return respective index
        if(dir[i].type == -1){
            return i;
        }
    }

    // If no spots are empty, return -1
    return -1;
}


// Function to calculate the size in bytes of the directory entries
uint64_t calculate_size_in_bytes(uint64_t num_entries) {
    return sizeof(DirectoryEntry) * num_entries;
}

// Function to calculate the number of blocks needed for the given size in bytes
uint64_t calculate_size_in_blocks(uint64_t size_in_bytes, uint64_t blockSize) {
    return (size_in_bytes + (blockSize - 1)) / blockSize;
}

// Function to recalculate the number of directory entries that can fit into the allocated blocks
uint64_t recalculate_num_dir_entries(uint64_t size_in_blocks, uint64_t blockSize) {
    return (size_in_blocks * blockSize) / sizeof(DirectoryEntry);
}

// Function to recalculate direcotry related variables
void calc_directory_sizes(uint64_t blockSize, uint64_t *size_in_bytes,
                          uint64_t *size_in_blocks, uint64_t *num_dir_entries) {
    *size_in_bytes = calculate_size_in_bytes(N_DIR_ENTRIES);
    *size_in_blocks = calculate_size_in_blocks(*size_in_bytes, blockSize);
    *num_dir_entries = recalculate_num_dir_entries(*size_in_blocks, blockSize);
    *size_in_bytes = calculate_size_in_bytes(*num_dir_entries); // Adjust size_in_bytes to fit exact number of DirectoryEntry structures
}

// Helper function to set the times
void set_times(DirectoryEntry *created_entry) {
    time(&created_entry->time_created);
    created_entry->modified_time = created_entry->time_created;
    created_entry->time_accessed = created_entry->time_created;
}

void create_dir_entry(DirectoryEntry* created_entry, const char* name, 
int type, uint64_t position, uint64_t size_entry, uint64_t entry_blocks){
   
   // Check for NULL pointer
    if (created_entry == NULL) return; 

    // Use strncpy to prevent buffer overflow and ensure null termination
    strncpy(created_entry->name, name, sizeof(created_entry->name) - 1);

    // Ensure null termination
    created_entry->name[sizeof(created_entry->name) - 1] = '\0';

    // Store respective variables
    created_entry->type = type;
    created_entry->starting_block = position;
    created_entry->file_size = size_entry;
    created_entry->number_blocks = entry_blocks;

    // Set the creation, modification, and access times
    set_times(created_entry);
}



// Modular function to initialize directory structure in memory
DirectoryEntry *allocate_directory(uint64_t size_in_blocks, uint64_t blockSize, uint64_t num_dir_entries) {
    
    // Mallocing space for directory
    DirectoryEntry *prepared_dir = malloc(size_in_blocks * blockSize);
    
    // Check for memory allocation failure
    if (!prepared_dir) {
        perror("memory failure in allocate_directory");
        exit(EXIT_FAILURE);
    }

    memset(prepared_dir, 0, size_in_blocks * blockSize);

    // Set all entries to unused
    for (int i = 0; i < num_dir_entries; i++) {
        prepared_dir[i].type = -1;
    }

    return prepared_dir;
}


// Function to configure the current and parent directory entries
void configure_directory_entries(DirectoryEntry *config_dir, DirectoryEntry *set_as_parent, 
            uint64_t position_of_directory, uint64_t size_in_bytes, uint64_t size_in_blocks) {

    // "." directory
    create_dir_entry(&config_dir[0], ".", FS_TYPE_DIR, position_of_directory, size_in_bytes, size_in_blocks);

    // ".." directory
    create_dir_entry(&config_dir[1], "..", FS_TYPE_DIR, 
                   set_as_parent ? set_as_parent->starting_block : position_of_directory,
                   size_in_bytes, size_in_blocks);
}

// Function to set up a directories variables and allocate space for it
uint64_t init_directory(DirectoryEntry *parentDirTable, DirectoryEntry *set_as_parent, const char *name) {
    uint64_t size_in_bytes, size_in_blocks, num_dir_entries;
    calc_directory_sizes(block_size, &size_in_bytes, &size_in_blocks, &num_dir_entries);

    DirectoryEntry *dir = allocate_directory(size_in_blocks, block_size, num_dir_entries);

    int64_t *free_blocks = get_free_blocks(size_in_blocks);
    int64_t position_of_directory = free_blocks[0];
    free(free_blocks);
    if (position_of_directory < 0) {
        perror("No space to allocate directory");
        exit(EXIT_FAILURE);
    }

    configure_directory_entries(dir, set_as_parent, position_of_directory, size_in_bytes, size_in_blocks);

    if (parentDirTable) {
        int index = find_empty_spot(parentDirTable);
        if (index == -1) {
            printf("ERROR: no empty spot in parent directory\n");
            return -1;
        }
        create_dir_entry(&parentDirTable[index], name, FS_TYPE_DIR, position_of_directory, size_in_bytes, size_in_blocks);
        LBAwrite(parentDirTable, set_as_parent->number_blocks, set_as_parent->starting_block);
    }

    LBAwrite(dir, size_in_blocks, position_of_directory);
    return position_of_directory;
}

// Set up root directory, setting its variables
uint64_t init_root_directory() {
    // initialize directory array
    initialize_dcbArray();

    // return the position of the root directory for VCB
    uint64_t size_in_bytes;
    uint64_t size_in_blocks;
    uint64_t num_dir_entries;

    calc_directory_sizes(block_size, &size_in_bytes, &size_in_blocks, &num_dir_entries);

    int64_t position = init_directory(NULL, NULL, NULL);
    dcbArray[0].startingBlock = position;

    // fill in fake parent directory entry for root
    create_dir_entry(&root_dir_entry, "/",
                   FS_TYPE_DIR, position,
                   size_in_bytes,
                   size_in_blocks);
    dcbArray[0].thisDirectory = &root_dir_entry;

    // read root directory from disk
    root_dir = read_directory(&root_dir_entry);
    dcbArray[0].dirTable = root_dir;

    return position;
}

DirectoryEntry * read_directory(DirectoryEntry *newEntry) {

    // Check if we already have this directory in cache
    fflush(stdout);
    b_dcb *dcb = find_dcb_for_starting_block(newEntry->starting_block);

    if (dcb == NULL) {
        dcb = find_free_dcb();
        if (dcb == NULL) {
            printf("ERROR: no free DCB\n");
            return NULL;
        }
        dcb->startingBlock = newEntry->starting_block;
        dcb->thisDirectory = newEntry;
    }

    if (dcb->dirTable == NULL) {
        dcb->dirTable = malloc(newEntry->number_blocks * block_size);
        LBAread(dcb->dirTable, newEntry->number_blocks, newEntry->starting_block);
    }

    return dcb->dirTable;
}


DirectoryEntry * write_to_disk(DirectoryEntry *thisDirectory, DirectoryEntry *dirTable){
    LBAwrite(dirTable, thisDirectory->number_blocks, thisDirectory->starting_block);
    return thisDirectory->starting_block;

}


int64_t read_root_directory(uint64_t position) {
    
    if (root_dir != NULL) {
        free(root_dir);
        root_dir = NULL;
    }

    // Block size needs to be dynamic, not fixed
    root_dir = malloc(block_size); 
    if (root_dir == NULL) {
        perror("Failed to allocate memory for reading root directory");
        exit(EXIT_FAILURE);
    }
    // Initialize memory
    memset(root_dir, 0, block_size); 

    // Read block from position into root_dir
    if (LBAread(root_dir, 1, position) != 1) { 
        // Read one from starting position
        perror("Failed to read root directory from disk");
        free(root_dir);
        root_dir = NULL;
        exit(EXIT_FAILURE);
    }

    free(root_dir);
    root_dir = NULL;
    return position;
}




// This function converts a given relative or absolute path into an absolute path
// based on the current working directory (PWD).
char *all_path_to_abs_path(const char *pathname) {
    
    // Pathname error check
    if (pathname == NULL) {
        fprintf(stderr, "Error: Null pathname provided.\n");
        return NULL;
    }

    // Already an absolute path
    if (pathname[0] == '/') {  
        return strdup(pathname);
    }

    // Calculate needed buffer size
    int is_root = (strcmp(PWD, "/") == 0);
    int buffer_size = strlen(PWD) + strlen(pathname) + 2; // For potential '/' and null terminator

    char *abs_path = malloc(buffer_size);

    // Memory allocation check
    if (abs_path == NULL) {
        perror("Memory allocation failed for absolute path");
        return NULL;
    }

    // Start with the current working directory
    strcpy(abs_path, PWD); 

    // Append '/' if PWD is not the root directory
    if (!is_root) {
        strcat(abs_path, "/"); 
    }

    // Append the relative path part
    strcat(abs_path, pathname); 

    return abs_path;
}

// This function resolves a pathname to a DirectoryEntry.
// It supports navigating through the directory structure using absolute paths.
DirectoryEntry *lookup_path(const char *pathname) {

    // Pathname error check
    if (pathname == NULL) {
        fprintf(stderr, "Error: Null pathname provided.\n");
        return NULL;
    }

    char *abs_pathname = all_path_to_abs_path(pathname);

    // Absolute pathname error check
    if (abs_pathname == NULL) {
        fprintf(stderr, "Error: Failed to convert path to absolute path.\n");
        return NULL;
    }

    char *working_pathname = strdup(abs_pathname);

    // Working pathname error check
    if (working_pathname == NULL) {
        perror("Memory allocation failed for working pathname");
        free(abs_pathname);
        return NULL;
    }

    // Tokenizing pathname
    char *tokens[MAX_NESTED_DIRS];
    char *last_ptr = NULL;
    char *token = strtok_r(working_pathname, "/", &last_ptr);
    int token_count = 0;

    // Storing tokenized pathnames to array
    while (token != NULL && token_count < MAX_NESTED_DIRS) {
        tokens[token_count++] = token;
        token = strtok_r(NULL, "/", &last_ptr);
    }

    // Free the absolute path string as it's no longer needed
    free(abs_pathname); 

    // Handle root directory case
    if (token_count == 0) {

        free(working_pathname);

        // Assuming root_dir is the global pointer to root DirectoryEntry
        return root_dir; 
    }

    // Create a stack to keep track of directory navigation
    DirectoryEntry **stack = malloc(sizeof(DirectoryEntry *) * (token_count + 1));

    // Memory allocation check
    if (stack == NULL) {
        perror("Memory allocation failed for directory stack");
        free(working_pathname);
        return NULL;
    }

    // Start at the root directory
    stack[0] = root_dir; 
    int stack_index = 0;

    // Navigate through each token
    for (int i = 0; i < token_count; i++) {

        if (strcmp(tokens[i], ".") == 0) {

            // Skip the current directory token
            continue; 

        } else if (strcmp(tokens[i], "..") == 0) {

            // Navigate up in the directory stack
            if (stack_index > 0) {
                stack_index--;
            }

            continue;
        } else {

            // Lookup the next directory in the path
            DirectoryEntry *current_dir = read_directory(stack[stack_index]);

            if (current_dir == NULL) {
                fprintf(stderr, "Failed to read directory during path lookup.\n");
                break;
            }

            DirectoryEntry *entry = lookup_name_in_dir(current_dir, tokens[i]);

            if (entry == NULL) {
                fprintf(stderr, "Directory entry '%s' not found.\n", tokens[i]);
                break;
            }

            stack_index++;
            stack[stack_index] = entry;
        }
    }

    // The final directory entry resolved by the path
    DirectoryEntry *result = stack[stack_index]; 

    free(working_pathname);
    free(stack);

    return result;
}

// Get the directory entry for a given pathname
DirectoryEntry * lookup_name_in_dir(DirectoryEntry *dir, const char *name) {
    
    if (dir == NULL) {
        return NULL;
    }

    int num_dir_entries = dir->file_size / sizeof(DirectoryEntry);

    // Search through directories
    for (int i = 0; i < num_dir_entries; i++) {
        if (dir[i].type != -1 && strcmp(dir[i].name, name) == 0) {
            return &dir[i];
        }
    }

    return NULL;
}

char*  get_parent_dir_path(const char * pathname) {
    // "/foo/bar" -> "/foo"
    int len=strlen(pathname);
    int i;

    // Get the index where the first slash located
    for (i=len-1; i>=0;i--){
        if(pathname[i] == '/'){
            break;
        }
    }

    if(i < 0){
        return NULL;
    }

    // Memory allocation and storing
    char * parentPath = malloc (i+2);
    strncpy(parentPath,pathname,i+1);
    parentPath[i+1]='\0';

    return parentPath;
}

const char* get_child_dir_name(const char * pathname) {
    // "/foo/bar" -> "bar"

    // Get the last part of the path name by phasing the slash
    const char* dirName = strrchr(pathname , '/');

    // If nothing found, return the name back.
    if(dirName == NULL){
        return pathname;
    }

    // +1 for slash
    return dirName +1;
}



// Create a new directory
int create_directory(const char * pathname){

    // Set up values for new directory
    uint64_t size_in_bytes = 0;
    uint64_t size_in_blocks = 0;
    uint64_t num_dir_entries = 0;

    calc_directory_sizes(block_size,&size_in_bytes,&size_in_blocks,&num_dir_entries);

    // Store related variables (parent path and its respective name)
    char *parentPath = get_parent_dir_path(pathname);
    const char *dirName = get_child_dir_name(pathname);

    if (parentPath == NULL) {

        parentPath = PWD;

    } else if (parentPath[0] != '/') {

        char *path = malloc(strlen(PWD) + strlen(parentPath) + 1);
        strcpy(path, PWD);
        strcat(path, parentPath);
        parentPath = path;

    }

    // Find parent directory table and parent directory entry
    DirectoryEntry * parentDirEntry = lookup_dir(parentPath);

    if (parentDirEntry == NULL) {
        printf("parent directory not found 1\n");
        return -1;
    }

    DirectoryEntry * parentDirTable = read_directory(parentDirEntry);

    // Check if directory already exists
    if (lookup_name_in_dir(parentDirTable, dirName) != NULL) {
        printf("directory already exists\n");
        return -1;
    }

    init_directory(parentDirTable, parentDirEntry, dirName);

     printf("directory created at path: %s %s\n", pathname, PWD);

    return 0;
}

DirectoryEntry * lookup_file(const char *pathname) {

    DirectoryEntry *entry = lookup_path(pathname);

    if (entry == NULL) {
        return NULL;
    }

    if (entry->type != FS_TYPE_FILE) {

        printf("returning because entry->type != FS_TYPE_FILE\n");
        return NULL;
    }

    return entry;
}


// Find directory entry
DirectoryEntry * lookup_dir(const char *pathname) {

    DirectoryEntry *entry = lookup_path(pathname);

    if (entry == NULL) {
        return NULL;
    }

    if (entry->type != FS_TYPE_DIR) {
        return NULL;
    }

    return entry;
}



int remove_directory(const char* path) {

    // INCOMPLETE

    return 0;
}

char* get_parent_filepath(const char* filepath) {
    char* lastSlash = strrchr(filepath, '/');
    if (lastSlash == NULL || lastSlash == filepath) {
        // Root or malformed path
        return strdup("/"); 
    }

    size_t length = lastSlash - filepath;
    char* parentPath = malloc(length + 1);

    // Memory allocation check
    if (parentPath == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    strncpy(parentPath, filepath, length);
    parentPath[length] = '\0';
    return parentPath;
}

char* get_child_filepath(const char* filepath) {

    char* lastSlash = strrchr(filepath, '/');

    if (lastSlash == NULL) {

        // No parent path present
        return strdup(filepath); 
    }

    return strdup(lastSlash + 1);
}