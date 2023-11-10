#ifndef INCLUDE_HEAP_H
#define INCLUDE_HEAP_H

#pragma once
#include "types.h"

#define NULL (void*)0
#define FALSE 0
#define TRUE !FALSE

// a singly linked list heap block
typedef struct _heapBlock {
    struct {
        uint32 size;  // memory size
        uint8 isFree; // block is free or not
    } metadata;
    struct _heapBlock *next; 
    void *data;  // data pointer
} HEAP_BLOCK;

int init(void* startAddr, void* endAddr);
void *brk(int size);
void* malloc(unsigned int sizeInBytes);
void free(void* addr);
// void heapPrintBlocks();

#endif /* INCLUDE_HEAP_H */