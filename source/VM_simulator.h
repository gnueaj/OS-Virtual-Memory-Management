#ifndef VMSIMULATOR_H
#define VMSIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum _strategies{
    MIN, LRU, LFU, WS
}Strategies;

typedef struct _table{
    int page;
    int inMemory[101];
    bool isPageFault;
}Table;

extern Table stateTable[4][1001];
extern int Qws[1001];
extern int frameCount[1001];
extern int pageFaultCount[4];

extern int N, M, W, K; 
extern int *refString;        
extern int *memState;       
extern int *refTime;
extern int *refCount;

bool readInput(); 
void VM_simulation();
void printAllTable();

#endif