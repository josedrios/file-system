/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Gursimran Singh, Cole Brausen, Jose Rios
* Student IDs:: 922759471, 918418987, 921160471
* GitHub-Name:: guri408, cb-1881, Colorbomb1
* Group-Name:: Team 3
* Project:: Basic File System
*
* File:: mfs.c
*
* Description:: This
*
**************************************************************/

#include "mfs.h"
#include "DirectoryEntry.h"
#include "VCB.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char * PWD = NULL;


char* combine_paths(char *base, const char *filename) {

    // Memory allocation
    char *path = malloc(strlen(base) + strlen(filename) + 2);

    strcpy(path, base);
    strcat(path, "/");
    strcat(path, filename);

    return path;
}

char *absolute_path(const char *pathname) {
    if (!pathname) {
        fprintf(stderr, "Error: Null pathname provided to absolute_path.\n");
        return NULL;
    }

    if (pathname[0] == '/') {
        return strdup(pathname);
    }

    
    int needed_size = strlen(PWD) + strlen(pathname) + 2; 
    char *full_path = malloc(needed_size);
    if (!full_path) {
        perror("Failed to allocate memory for full path");
        return NULL;
    }

    // Ensure no double slash at root
    if (strcmp(PWD, "/") == 0) {
        snprintf(full_path, needed_size, "/%s", pathname);
    } else {
        snprintf(full_path, needed_size, "%s/%s", PWD, pathname);
    }

    return full_path;
}


// create directory
int fs_mkdir(const char *pathname, mode_t mode) {
    return create_directory(pathname);
}

// Remove a directory
int fs_rmdir(const char *pathname) {
    if (!pathname) {
        fprintf(stderr, "Invalid pathname provided.\n");
        return -1;
    }

    return remove_directory(pathname);
}

fdDir *fs_opendir(const char *pathname) {
    DirectoryEntry *entry = lookup_path(pathname);

    if (entry == NULL) {
        return NULL;
    }

    fdDir *dir = malloc(sizeof(fdDir));
    if (dir == NULL) {
        printf("fail to do malloc");
        return NULL;
    }
    dir->d_reclen = sizeof(fdDir);
    dir->dirEntryPosition = 0;
    dir->directoryStartLocation = entry->starting_block;
    dir->thisDirectory = entry;
    dir->dirTable = read_directory(entry);
    dir->path = absolute_path(pathname);
    return dir;
}


struct fs_diriteminfo *fs_readdir(fdDir *dirp) {
    int i;
    int num_dir_entries = dirp->thisDirectory->file_size / sizeof(DirectoryEntry);
    for (i = dirp->dirEntryPosition; i < num_dir_entries; i++) {
        if (dirp->dirTable[i].type != FS_TYPE_UNUSED) {
            dirp->dirEntryPosition = i;
            break;
        }
    }
    if (i >= num_dir_entries) {
        return NULL;
    }
    struct fs_diriteminfo *item = malloc(sizeof(struct fs_diriteminfo));
    DirectoryEntry *entry = &dirp->dirTable[dirp->dirEntryPosition];
    strcpy(item->d_name, entry->name);
    item->fileType = entry->type;
    item->d_reclen = sizeof(struct fs_diriteminfo);
    dirp->dirEntryPosition++;
    return item;
}

int fs_closedir(fdDir *dirp) {
    free(dirp->path);
}

char *fs_getcwd(char *pathname, size_t size) {
    if (PWD == NULL) {
        return NULL;
    }

    if (size < strlen(PWD)) {
        return NULL;
    }
    strcpy(pathname, PWD);
    return pathname;
}

int fs_setcwd(char *pathname) {
    char *abs_path = absolute_path(pathname);

    DirectoryEntry *dir = lookup_path(abs_path);

    if (dir == NULL) {
        printf("Directory entry does not exist.\n");
        free(abs_path);
        return -1;
    }

    if (dir->type != FS_TYPE_DIR) {
        printf("Directory entry is not file, return error!\n");
        free(abs_path);
        return -1;
    }

    PWD = abs_path;
    return 0;
}

// returns boolean value
int fs_isFile(char *filename) {
    DirectoryEntry *entry = lookup_file(filename);
    return entry != NULL;
}


int fs_isDir(char *pathname) {
    DirectoryEntry *entry = lookup_dir(pathname);
    return entry != NULL;
}

int fs_delete(char* filename) {
    
    return -1;
}

int fs_stat(const char *path, struct fs_stat *buf) {
    char *abs_path = absolute_path(path);
    DirectoryEntry *entry = lookup_path(abs_path);

    if (entry == NULL) {
        return -1;
    }
    buf->st_size = entry->file_size;
    buf->st_blocks = entry->number_blocks;
    buf->st_blksize = block_size;
    buf->st_createtime = entry->time_created;
    buf->st_accesstime = entry->time_accessed;
    buf->st_modtime = entry->modified_time;
    free(abs_path);
    return 0;
}
