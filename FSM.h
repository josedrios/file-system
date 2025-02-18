/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Gursimran Singh, Cole Brausen, Jose Rios
* Student IDs:: 922759471, 918418987, 921160471
* GitHub-Name:: guri408, cb-1881, Colorbomb1
* Group-Name:: Team 3
* Project:: Basic File System
*
* File:: FSM.h
*
* Description:: Contains functions related to our free space
* management in list form
*
**************************************************************/

// Libraries
#include "VCB.h"

// Globals
#define NUMBER_OF_BITS 8
#define NUMBER_OF_BYTES 4096

// Free space manager operations
int64_t init_free_space_map(VCB * volume);
int64_t write_free_space_map();
int64_t write_free_space_map_block(uint64_t index);

// General operations regarding block functionality
int64_t read_free_space_map(uint64_t position, uint64_t count);
int64_t * get_free_blocks(uint64_t count);
int64_t find_free_blocks(uint64_t count);
int64_t * new_get_blocks_for_file(uint64_t startingBlock,
                                  uint64_t currentBlockCount,
                                  uint64_t sizeInBlocks);
int64_t free_blocks(uint64_t count, uint64_t positions);


int64_t read_file(uint32_t startingBlock, uint64_t countBlocks, void * data);
int64_t write_file(uint32_t startingBlock, uint64_t countBlocks, void * data);

// Helper functions
int64_t mark_block_free(uint64_t position);  
int64_t mark_block_used(uint64_t position); 
int64_t is_free(uint64_t index);           