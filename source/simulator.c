#include "VM_simulator.h"

#define INF_DIST 1001

Table stateTable[4][1001];
int Qws[1001]; // Q working set
int frameCount[1001]; // # of frames allocated
int pageFaultCount[4]; // # of page fault

void stateInit();
void ST_FA(Strategies strategy); // Fixed Allocation

int ST_MIN(int curTime); // Return memory state index using MIN algorithm
int ST_LRU(void); // Return memory state index using LRU algorithm
int ST_LFU(void); // Return memory state index using LFU algorithm
void ST_WS(); // Variable allocation using WS algorithm

void stateInit()
{
    for(int i = 0; i < N; i++) refCount[i] = 0;
    for(int i = 0; i < M; i++) memState[i] = refTime[i] = -1; 
    for(int i = 0; i <= K; i++) Qws[i] = -1;
}

void VM_simulation()
{
    ST_FA(MIN);
    ST_FA(LRU);
    ST_FA(LFU);
    ST_WS();
}

void ST_FA(Strategies strategy)
{
    stateInit();
    int pageFault = 0; // page fault count
    for(int i = 0; i < K; i++) 
    {
        int curTime = i + 1; // current Time
        int curPage = refString[i]; // current page
        bool isPageFault = true; // check if the page is in memory
        stateTable[strategy][curTime].page = curPage; 
        stateTable[strategy][curTime].isPageFault = true;
        // filling state table
        refCount[curPage] += 1;

        for(int j = 0; j < M; j++) 
        {
            if(memState[j] == curPage) // Already exists in page frame
            {
                isPageFault = false;
                stateTable[strategy][curTime].isPageFault = false;
                refTime[j] = curTime;
                break;
            }
        }

        // Page fault handling
        if(isPageFault) 
        {
            pageFault += 1;  
            int replaceIdx;
            for(int j = 0; j < M; j++) 
            {
                if(memState[j] == -1) // Space already exists to replace
                {
                    replaceIdx = j;
                    break;
                }
                else if(j == M - 1) // implement replacement strategies
                {
                    switch (strategy) 
                    {
                        case MIN:
                            replaceIdx = ST_MIN(i);
                            break;
                        case LRU:
                            replaceIdx = ST_LRU();
                            break;
                        case LFU:
                            replaceIdx = ST_LFU();
                            break;
                        default:   
                            break;
                    }
                }
            }
            memState[replaceIdx] = curPage; // page state update
            refTime[replaceIdx] = curTime; // reference time update
        }
        for(int j = 0; j < M; j++) // state table update
            stateTable[strategy][curTime].inMemory[j] = memState[j];
    }
    pageFaultCount[strategy] = pageFault;
}

int ST_MIN(int curTime) 
{
    int Df[21]; // Storing forward distance 
    for(int i = 0; i < M; i++) 
        Df[i] = INF_DIST;  // Set initial distance infinite
    
    for(int i = 0; i < M; i++) // Set initial distance
    {
        int page = memState[i];
        for(int j = curTime + 1; j < K; j++) 
        {
            if(page == refString[j]) 
            {
                Df[i] = j;
                break;
            }
        }
    }

    // Find max forward distance
    int maxDfIdx = 0;
    int maxDf = Df[maxDfIdx];
    for(int i = 1; i < M; i++) 
    {
        if(Df[i] == INF_DIST && memState[i] < memState[maxDfIdx]) 
        {// select lower number of page(tie breaking)
            maxDfIdx = i;
            maxDf = Df[maxDfIdx];
        }
        else if(maxDf != INF_DIST && Df[i] > maxDf) 
        {
            maxDfIdx = i;
            maxDf = Df[maxDfIdx];
        }
    }
    return maxDfIdx;
}

int ST_LRU() 
{
    int idx = 0;
    int LRU_page = refTime[idx];
    for(int i = 1; i < M; i++) 
    {
        if(refTime[i] < LRU_page) 
        {
            idx = i;
            LRU_page = refTime[idx];
        }
    }
    return idx;
}

int ST_LFU()
{
    int LFU_page = memState[0];
    int minRef = refCount[LFU_page];
    int idx = 0;
    // initial state

    for(int i = 1; i < M; i++)
    {
        LFU_page = memState[i];
        if(refCount[LFU_page] < minRef) 
        {
            idx = i;
            minRef = refCount[LFU_page];
        }
        else if(refCount[LFU_page] == minRef) 
        {// using LRU algorithm if they have same reference count (tie breaking)
            if(refTime[i] < refTime[idx]) 
            {
                idx = i;
                minRef = refCount[LFU_page];
            }
        }
    }
    return idx;
}

void ST_WS() 
{
    stateInit();
    int pageFault = 0;
    int workingSet[101] = {0, };

    for(int i = 0; i < K; i++) 
    {
        int curTime = i + 1;
        int curPage = refString[i]; 
        stateTable[WS][curTime].page = curPage;
        stateTable[WS][curTime].isPageFault = true;

        for(int j = 0; j < N; j++)
        { 
            if(workingSet[j] == 1) 
                workingSet[j] = -1;
        }// store precedence state

        for(int j = 1; j <= W; j++) 
        {
            if(i - j >= 0) 
                workingSet[refString[i - j]] = 1;
        }// store [t-windowSize, t]

        // check pagefault and update state
        if(workingSet[curPage] != 0) 
            stateTable[WS][curTime].isPageFault = false;
        else 
            pageFault += 1;
        workingSet[curPage] = 1;

        for(int j = 0; j < N; j++) 
        {
            if(workingSet[j] == -1) 
            {
                workingSet[j] = 0;
                Qws[curTime] = j; 
            }
            stateTable[WS][curTime].inMemory[j] = workingSet[j];
            frameCount[curTime] += workingSet[j];
            // Update table and number of frames allocated
        }
    }
    pageFaultCount[WS] = pageFault;
}