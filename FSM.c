/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Gursimran Singh, Cole Brausen, Jose Rios
* Student IDs:: 922759471, 918418987, 921160471
* GitHub-Name:: guri408, cb-1881, Colorbomb1
* Group-Name:: Team 3
* Project:: Basic File System
*
* File:: FSM.c
*
* Description:: Contains functions related to our free space
* management
*
**************************************************************/

// Libraries
#include "VCB.h"
#include "FSM.h"
#include "fsLow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

u_int8_t * freeSpaceMap = NULL;
uint64_t blocks_freespace;

// Function to calculate the number of blocks needed for the free space map
void calculate_free_space_blocks(VCB *volume) {
    uint64_t freeSpaceBitsPerBlock = volume->block_size * NUMBER_OF_BITS; 
    volume->blocks_freespace = volume->block_number / freeSpaceBitsPerBlock;
    if (volume->block_number % freeSpaceBitsPerBlock != 0) {
        volume->blocks_freespace++;
    }
    printf("Free space blocks: %ld\n", volume->blocks_freespace);
}

// Function to allocate memory for the free space map
int allocate_free_space_map(VCB *volume) {
    freeSpaceMap = malloc(volume->blocks_freespace * volume->block_size);
    if (freeSpaceMap == NULL) {
        // Memory allocation failed
        fprintf(stderr, "Failed to allocate memory for free space map.\n");
        return -1; 
    }
    memset(freeSpaceMap, 0xFF, volume->blocks_freespace * volume->block_size); // Initially mark all as free
    return 0; // Success
}

// Function to mark system-reserved blocks as used
void mark_system_blocks_used(VCB *volume) {
    for (int64_t i = 0; i < 1 + volume->blocks_freespace; i++) {
        mark_block_used(i);
    }
}

// Function to initialize the free space map
int64_t init_free_space_map(VCB *volume) {
    if (volume == NULL) return -1; // Check for null pointer

    calculate_free_space_blocks(volume);
    volume->position_freespace = 1; // Set the starting position for free space
    blocks_freespace = volume->blocks_freespace;

    if (allocate_free_space_map(volume) != 0) {
        return -1; // Return error if memory allocation fails
    }

    mark_system_blocks_used(volume);

    if (write_free_space_map() != 0) {
        return -1; // Check for errors in writing the map to disk
    }

    return 0; // Success
}

// write free space map to disk
int64_t write_free_space_map() {
    LBAwrite(freeSpaceMap, blocks_freespace, 1);
}


// compute the offset within the free space map
u_int8_t *getFreeSpaceMapOffset(uint64_t index) {
    return freeSpaceMap + index * block_size;
}

// write a particular block of the free space map to disk
int64_t write_free_space_map_block(uint64_t index) {
    return LBAwrite(getFreeSpaceMapOffset(index), 1, 1 + index);
}


// read free space map from disk
int64_t read_free_space_map(uint64_t position, uint64_t count) {
    blocks_freespace = count;
    freeSpaceMap = malloc(blocks_freespace * block_size);
    LBAread(freeSpaceMap, blocks_freespace, position);
}

// Function to set a range of blocks as free using a while loop
int64_t set_free(uint64_t count, uint64_t startingBlock) {
    int64_t free_block_count = 0;
    uint64_t i = 0;  // index of block position

    while (i < count) {
        int64_t position = startingBlock + i;
        mark_block_free(position);  // Assumes mark_block_free modifies freeSpaceMap directly
        free_block_count++;
        i++;  
    }

    return free_block_count;
}


// Function to free up blocks at positions and write changes to disk
int64_t free_blocks(uint64_t count, uint64_t startingBlock) {
    // Mark blocks at positions as free in map
    int64_t freed_blocks = set_free(count, startingBlock);

    // Write the updated free space map to disk
    write_free_space_map();

    return freed_blocks;
}

// Function to check if a range of blocks starting from a given position are all free
int are_blocks_free(uint64_t start, uint64_t count) {
    for (uint64_t i = start; i < start + count; i++) {
        if (!is_free(i)) {
            return 0;
        }
    }
    return 1;
}


int64_t find_first_free_block(int64_t map_len) {
    for (int64_t i = 0; i < map_len; i++) {
        if (is_free(i)) {
            return i;  // Return the index of the first free block
        }
    }
    return -1;  // No free block found
}

int verify_contiguous_free_blocks(int64_t start_index, uint64_t count, int64_t map_len) {
    if (start_index == -1) return 0;  // Invalid start index
    //check to see if not all blocks in the range are free
    for (int64_t i = start_index; i < start_index + count; i++) {
        if (i >= map_len || !is_free(i)) {
            return 0; 
        }
    }
    return 1;  // All blocks in the range are free
}

int64_t find_free_blocks(uint64_t count) {
    int64_t map_len = blocks_freespace * block_size * NUMBER_OF_BITS; 
    int64_t start_block = find_first_free_block(map_len);
    int64_t free_block_condition = (start_block != -1 && start_block);


    if (start_block == NULL){

        printf("starting block not found in find_free_blocks");
        return 999;
    }

    do {
        if (verify_contiguous_free_blocks(start_block, count, map_len)) {
            return start_block;  // Found a valid range of free blocks
        }
        // Continue scanning from the next block after the last checked start
        start_block = find_first_free_block(start_block + 1);
    } while (free_block_condition <= map_len - count);  // Condition checked at the end of the loop


    return -1;  // No contiguous free blocks found
}



//fill position with markfree

void mark_blocks_and_fill_positions(int64_t start_block, uint64_t count, int64_t *positions) {
    for (int64_t i = 0; i < count; i++) {
        int64_t position = start_block + i;
        positions[i] = position;
        mark_block_used(position);
    }
    positions[count] = -1; // End marker
}

int64_t *get_free_blocks(uint64_t count) {
    int64_t *positions = malloc(sizeof(int64_t) * (count + 1));
    if (positions == NULL) {
        return NULL; // Handle malloc failure
    }

    int64_t start_block = find_free_blocks(count);
    if (start_block == -1) {
        free(positions);
        return NULL;
    }

    mark_blocks_and_fill_positions(start_block, count, positions);
    write_free_space_map();

    return positions;
}


int64_t mark_block_free(uint64_t position) {
    int64_t block_index = position / NUMBER_OF_BYTES;
    int64_t byte_index = position / NUMBER_OF_BITS;
    u_int8_t or_value = 1 << (position % NUMBER_OF_BITS);
    freeSpaceMap[byte_index] |= or_value;
    return 0;
}

int64_t mark_block_used(uint64_t position) {
    int64_t block_index = position / NUMBER_OF_BYTES;
    int64_t byte_index = position / NUMBER_OF_BITS;
    int64_t bit_position = position % NUMBER_OF_BITS;

    // 000010000 -> 111101111
    u_int8_t or_value = 1 << bit_position;
    u_int8_t mask_value = ~or_value;

    freeSpaceMap[byte_index] &= mask_value;
    return 0;
}

int64_t is_free(uint64_t index) {
    return freeSpaceMap[index/NUMBER_OF_BITS] & (1<<(index % NUMBER_OF_BITS));
}
