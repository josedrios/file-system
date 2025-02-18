/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Gursimran Singh, Cole Brausen, Jose Rios
* Student IDs:: 922759471, 918418987, 921160471
* GitHub-Name:: guri408, cb-1881, Colorbomb1
* Group-Name:: Team 3
* Project:: Basic File System
*
* File:: VCB.c
*
* Description:: Contains the operations needed related to our
* VCB (Volume Control Block)
*
**************************************************************/

// Libraries
#include <stdio.h>
#include "VCB.h"

// Global Block Size Variable of 512
uint64_t block_size;

// Confirm whether the signatures we have had errors
int check_signatures(VCB *volume) {

    // Determine whether the signatures match
    int isValid = verify_signatures(volume);

    // If signatures did not match, notify the user
    report_signature_status(volume, isValid);

    // Return signature verification result
    return isValid;
}


// Verify that the given signatures match our digital signatures
int verify_signatures(const VCB *volume) {
    
    // If signatures match their respective digital signatures return true, if else return false
    return (volume->dig_sign1 == DIGITAL_SIGNATURE1) && (volume->dig_sign2 == DIGITAL_SIGNATURE2);
}

// Notify user whether their signature verfication process passed or failed
void report_signature_status(const VCB *volume, int isValid) {
    
    // Check whether the signature verification process passed
    if (isValid) {

        // If verfication passed, notify user success
        printf("Digital signatures are valid.\n");
    } else {

        // If verifaction failed, notify user of failure
        printf("signature check: signature 1 = %ld(%ld), signature 2 = %ld(%ld)\n",
               volume->dig_sign1, DIGITAL_SIGNATURE1,
               volume->dig_sign2, DIGITAL_SIGNATURE2);
        printf("Digital signatures are invalid!\n");
    }
}

// Set up the volume control block for utilization by storing known variables in struct 
int init_volume_control_block(VCB *volume, uint64_t block_number, uint64_t init_block_size) {
    
    // Storing signature variables
    volume->dig_sign1 = DIGITAL_SIGNATURE1;
    volume->dig_sign2 = DIGITAL_SIGNATURE2;

    // Block Related Variables
    volume->block_number = block_number;
    volume->block_size = init_block_size;

    // Current position in freespace
    volume->position_freespace = 1;

    // Notify user of current status of different variables of the VCB
    printf("Block Number Address: %p\n", volume->block_number);
    printf("Block Size Address: %p\n", volume->block_size);
    printf("position Address: %p\n", volume->position_freespace);

    return 0;
}

// Reading volume function 
uint64_t read_volume(VCB *volume) {

    // Utilizing LBAread to read volume
    uint64_t result = LBAread(volume, 1, 0);
    block_size = volume->block_size;

    return result;
}

// Writing volume function
uint64_t write_volume(VCB *volume) {

    // Notify User
    printf("Writing volume\n");

    // Utilizing LBAWrite to write to volume
    return LBAwrite(volume, 1, 0);
}

// TESTING PURPOSES ONLY
// Used to test our VCB initialization
void initialization_test(){

    // Declare volume
    VCB volume;
    
    // Store block related values
    uint64_t block_number = 40; 
    uint64_t init_block_size = 512; 

    // Initialize our VCB
    init_volume_control_block(&volume, block_number, init_block_size);
       
    // Signature checks
    if (check_signatures(&volume)) {
        printf("Digital signatures are valid. Happy day.\n");
    } else {
        printf("Digital signatures are invalid!\n");
    }

    // Write to volume
    write_volume(&volume);

}