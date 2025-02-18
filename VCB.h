/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Gursimran Singh, Cole Brausen, Jose Rios
* Student IDs:: 922759471, 918418987, 921160471
* GitHub-Name:: guri408, cb-1881, Colorbomb1
* Group-Name:: Team 3
* Project:: Basic File System
*
* File:: VCB.h
*
* Description:: Is the header file for our VCB. This file 
* contains the VCB struct and a list of the operations 
* for the VCB (Volume Control Block)
*
**************************************************************/


// Libraries
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fsLow.h"

#ifndef _VCB_H
#define _VCB_H

// Signatures 
#define DIGITAL_SIGNATURE1 0x6F75726673536967
#define DIGITAL_SIGNATURE2 0x7465616D32736967

// Block size value of 512
extern uint64_t block_size;

// Struct of our volume control block
typedef struct VCB{

   // Signatures
   int64_t dig_sign1;
   int64_t dig_sign2;

   // Block size
   int64_t block_size;

   // Amount of blocks in freespace
   int64_t block_number;

   // Position of the root directory
   uint64_t root_dir_pos;

   // Freespace manager members to work with VCB properly
   int64_t position_freespace;
   int64_t blocks_freespace;
   
} VCB;

// Signature overall check
int check_signatures(VCB * volume);

// Verify that signatures match the digital signatures
int verify_signatures(const VCB *volume);

// Notify user whether the signature verifcation process failed or passed
void report_signature_status(const VCB *volume, int isValid);

// Initializes the volume control block, storing known values
int init_volume_control_block(VCB *volume, uint64_t block_number, uint64_t block_size);

// Basic operations controlling our volume
uint64_t write_volume(VCB *volume);
uint64_t read_volume(VCB *volume);

#endif