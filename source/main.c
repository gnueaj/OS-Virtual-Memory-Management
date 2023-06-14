#include "VM_simulator.h"

int main() 
{
    /* 
    1. read input (in IO.c)
    2. simualtion (in simulator.c)
    3. print a table with results (in IO.cpp)
    */
    if(readInput())
    {
        VM_simulation();
        printAllTable();
    }
    return 0;
}