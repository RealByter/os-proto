#include "heap.h"

void *gHeapStartAddr = NULL, *gHeapEndAddr = NULL;
unsigned long gTotalSize = 0;
unsigned long gTotalUsedSize = 0;
HEAP_BLOCK* gHead = NULL;

int init(void* startAddr, void* endAddr) {
    if (startAddr > endAddr) {
        printf("failed to init heap\n");
        return -1;
    }
    gHeapStartAddr = startAddr;
    gHeapEndAddr = endAddr;
    gTotalSize = endAddr - startAddr;
    gTotalUsedSize = 0;
}

void *brk(int size) {
    void *addr = NULL;
    if (size <= 0)
        return NULL;
    // check memory is available or not
    if ((int)(gTotalSize - gTotalUsedSize) <= size)
        return NULL;
    // add start addr with total previously used memory and difference between each data block pointer
    addr = gHeapStartAddr + gTotalUsedSize + size + sizeof(void*);
    gTotalUsedSize += size + sizeof(void*);
    return addr;
}

HEAP_BLOCK *worstFit(int size) {
    HEAP_BLOCK *temp = gHead;
    while (temp != NULL) {
        if (isBlockFree(temp)) {
            if ((int)temp->metadata.size >= size)
                return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

int isBlockFree(HEAP_BLOCK *block) {
    if (!block)
        return FALSE;
    return (block->metadata.isFree == TRUE);
}

HEAP_BLOCK *allocate_new_block(int size) {
    HEAP_BLOCK *temp = gHead;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    HEAP_BLOCK *newBlock = (HEAP_BLOCK *)kbrk(sizeof(HEAP_BLOCK));
    newBlock->metadata.isFree = FALSE;
    newBlock->metadata.size = size;
    newBlock->data = kbrk(size);
    newBlock->next = NULL;
    temp->next = newBlock;
    return newBlock;
}

void* malloc(unsigned int size) {
    if (size <= 0)
        return NULL;
    if (gHead == NULL) {
        gHead = (HEAP_BLOCK *)kbrk(sizeof(HEAP_BLOCK));
        gHead->metadata.isFree = FALSE;
        gHead->metadata.size = size;
        gHead->next = NULL;
        gHead->data = kbrk(size);
        return gHead->data;
    } else {
        HEAP_BLOCK *worst = worst_fit(size);
        if (worst == NULL) {
            HEAP_BLOCK *newBlock = allocate_new_block(size);
            newBlock->metadata.isFree = FALSE;
            newBlock->metadata.size = size;
            newBlock->data = kbrk(size);
            return newBlock->data;
        } else {
            worst->metadata.is_free = FALSE;
            return worst->data;
        }
    }
    return NULL;
}

void free(void *addr) {
    HEAP_BLOCK *temp = gHead;
    while (temp != NULL) {
        if (temp->data == addr) {
            temp->metadata.isFree = TRUE;
            return;
        }
        temp = temp->next;
    }
}

void heapPrintBlocks() {
    HEAP_BLOCK *temp = gHead;
    printf("Block Size: %d\n", sizeof(HEAP_BLOCK));
    while (temp != NULL) {
        printf("size:%d, free:%d, data: 0x%x, curr: 0x%x, next: 0x%x\n",
               temp->metadata.size, temp->metadata.isFree, temp->data, temp, temp->next);
        temp = temp->next;
    }
}