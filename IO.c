#include "VM_simulator.h"

int N, M, W, K; // # of page, # of page frame, window size, p.ref string length
int *refString; // page reference string
int *memState; // storing memory state
int *refTime; // page reference time
int *refCount; // page reference count

bool readInput()
{
    /* Read input and catch exception */
    FILE* fp = fopen("input.txt", "r"); 

    /* Error1 : File open failed */
    if(fp == NULL)
    {
        printf("File open failed.\n");
        return false;
    }
    
    /* Error2 : Invalid input format */
    if (fscanf(fp, "%d %d %d %d", &N, &M, &W, &K) != 4)
    {
        printf("Please check the input format.\n");
        return false;
    }
    
    /* Error3 : Invalid input size */
    if(N <= 0 || M <= 0 || W < 0 || K <= 0 ||
        N > 100 || M > 20 || W > 100 || K > 1000)
    {
        printf("Please check the input size.\n");
        return false;
    }

    refString = (int*)malloc(sizeof(int) * K);
    memState = (int*)malloc(sizeof(int) * M);
    refTime = (int*)malloc(sizeof(int) * M);
    refCount = (int*)malloc(sizeof(int) * N);
    for(int i=0; i < K; i++)
    {
        if(fscanf(fp, "%d", &refString[i]) == EOF)
        { // Insufficient number of inputs
            printf("Please check the input format.\n");
            return false;
        }
        else if(refString[i] < 0 || refString[i] >= N)
        { // Invalid String number
            printf("Please check the page string.\n");
            return false;
        }
    }

    int tmp;
    if(fscanf(fp, "%d", &tmp) == 1)
    {// Number of inputs exceeded
        printf("Please check the input format.\n");
        return false;
    }
    return true;
}

void printLine()
{
    printf("+--------+");
    for(int i = 0; i < K; i++) printf("----+");
    printf("\n");
}

void printTable(Strategies replacement)
{
    switch(replacement)
    {
        case MIN:
            printf("MIN\n");
            break;
        case LRU:
            printf("LRU\n");
            break;
        case LFU:
            printf("LFU\n");
            break;
        case WS:
            printf("WS\n");
            break;
        default:
            break;
    }

    printf("The number of page faults: %d\n", pageFaultCount[replacement]);
    printLine();
    //print page fault 

    printf("|%7s |", "Time");
    for(int i = 1; i <= K; i++)
        printf("%4d|", i);
    printf("\n");
    printLine();
    //print time

    printf("|%7s |", "Page");
    for(int i = 1; i <= K; i++)
        printf("%4d|", stateTable[replacement][i].page);
    printf("\n");
    printLine();
    //print ref. string
    
    if(replacement == WS)
    {
        printf("|%7s |", "Memory");
        for(int j = 0; j < N; j++)
        {
            for(int i = 1; i <= K; i++)
            {
                int page = stateTable[replacement][i].inMemory[j];
                if(page == 0) printf("%4c|", '-');
                else printf("%4d|", j);
            }
            if(j != N - 1) printf("\n|%7s |", " ");
        }
    }
    else
    {
        printf("|%7s |", "Memory");
        for(int j = 0; j < M; j++)
        {
            for(int i = 1; i <= K; i++)
            {
                int page = stateTable[replacement][i].inMemory[j];
                if(page == -1) printf("%4c|", ' ');
                else printf("%4d|", page);
            }
            if(j != M - 1) printf("\n|%7s |", " ");
        }
    }
    printf("\n");
    printLine();
    //print memory state

    printf("|%7s |", "P.F");
    for(int i = 1; i <= K; i++)
        printf("%4c|", stateTable[replacement][i].isPageFault == true ? 'F' : ' ');
    printf("\n");
    if(replacement == WS)
    {
        printf("|%7s |", "Pws");
        for(int i = 1; i <= K; i++)
        {
            if(!stateTable[replacement][i].isPageFault) printf("%4c|", ' ');
            else printf("%4d|", stateTable[replacement][i].page);
        }
        printf("\n");

        printf("|%7s |", "Qws");
        for(int i = 1; i <= K; i++)
        {
            if(Qws[i] == -1) printf("%4c|", ' ');
            else printf("%4d|", Qws[i]);
        }
        printf("\n");

        printLine();
        printf("|%7s |", "Frames");
        for(int i = 1; i <= K; i++)
            printf("%4d|", frameCount[i]);
        printf("\n");
    }
    printLine();
    printf("\n");
    // print page fault, P working set, W working set, number of frames allocated
}

void printAllTable()
{
    printTable(MIN);
    printTable(LRU);
    printTable(LFU);
    printTable(WS);

    free(refString);
    free(memState);
    free(refTime);
    free(refCount);
}