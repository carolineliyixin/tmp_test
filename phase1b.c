/*
Phase 1b
*/

#include "phase1Int.h"
#include "usloss.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef struct PCB {
    int             cid;                // context's ID
    int             cpuTime;            // process's running time
    char            name[P1_MAXNAME+1]; // process's name
    int             priority;           // process's priority
    P1_State        state;              // state of the PCB
    // more fields here
    int used;
} PCB;

static PCB processTable[P1_MAXPROC];   // the process table
static int currentPid = -1;

static void IllegalInstructionHandler() {
    P1_Quit(1024);
}


/*
 * Help to check the mode
 */
void checkIfKernelMode() 
{
    unsigned int currentPSR = USLOSS_PsrGet();
    unsigned int mask = 1;

    if (!(currentPSR & mask)) {        // if user mode
        USLOSS_IllegalInstruction();
    } 
}

void P1ProcInit(void)
{
    USLOSS_IntVec[USLOSS_ILLEGAL_INT] = IllegalInstructionHandler;
    checkIfKernelMode();

    P1ContextInit();
    for (int i = 0; i < P1_MAXPROC; i++) {
        processTable[i].state = P1_STATE_FREE;
        // initialize the rest of the PCB
        processTable[i].cid = -1;
        processTable[i].cpuTime = 0;
        processTable[i].priority = 0;
        processTable[i].used = 0;
        *processTable[i].name = '\0';
    }
    // initialize everything else

}

int P1_GetPid(void) 
{
    return currentPid;
}

int P1_Fork(char *name, int (*func)(void*), void *arg, int stacksize, int priority, int tag, int *pid ) 
{
    // check for kernel mode
    checkIfKernelMode();
    
    // disable interrupts
    int is_enabled = P1DisableInterrupts();   

    // check all parameters
    if (tag != 0 && tag != 1) 
        return P1_INVALID_TAG;
    if (priority < 1 || priority > 6 || (processTable[0].used && priority > 5)) 
        return P1_INVALID_PRIORITY;
    if (stacksize < USLOSS_MIN_STACK)
        return P1_INVALID_STACK;
    if (!*name)
        return P1_NAME_IS_NULL;
    else if (sizeof(name) > P1_MAXNAME)
        return P1_NAME_TOO_LONG;
    
    // create a context using P1ContextCreate
    int cid;
    int rc;
    rc = P1ContextCreate((void *)func, arg, stacksize, &cid);
    assert(rc == P1_SUCCESS);
    
    // allocate and initialize PCB
    int i;
    for (i = 0; i < P1_MAXPROC; i++) {
        if (processTable[i].used && !strcmp(processTable[i].name, name))
            return P1_DUPLICATE_NAME;
        if (!processTable[i].used) {
            processTable[i].cid = cid;
            // processTable[i].cpuTime = 0;
            processTable[i].priority = priority;
            strcat(processTable[i].name, name);
            processTable[i].used = 1;
            
            *pid = i; 
            break;
        }
    }

    // if this is the first process or this process's priority is higher than the 
    //    currently running process call P1Dispatch(FALSE)
    if (i == 0 || priority > processTable[currentPid].priority)
        P1Dispatch(FALSE);

    // re-enable interrupts if they were previously enabled
    if (is_enabled)
        P1EnableInterrupts();

    if (i >= P1_MAXPROC)
        return P1_TOO_MANY_PROCESSES;
    else
        return P1_SUCCESS;    
}

void 
P1_Quit(int status) 
{
    // check for kernel mode
    // disable interrupts
    // remove from ready queue, set status to P1_STATE_QUIT
    // if first process verify it doesn't have children, otherwise give children to first process
    // add ourself to list of our parent's children that have quit
    // if parent is in state P1_STATE_JOINING set its state to P1_STATE_READY
    P1Dispatch(FALSE);
    // should never get here
    assert(0);
}


int 
P1GetChildStatus(int tag, int *cpid, int *status) 
{
    int result = P1_SUCCESS;
    // do stuff here
    return result;
}

int
P1SetState(int pid, P1_State state, int sid) 
{
    int result = P1_SUCCESS;
    // do stuff here
    return result;
}

void
P1Dispatch(int rotate)
{
    // select the highest-priority runnable process
    // call P1ContextSwitch to switch to that process
}

int
P1_GetProcInfo(int pid, P1_ProcInfo *info)
{
    int         result = P1_SUCCESS;
    // fill in info here
    return result;
}







